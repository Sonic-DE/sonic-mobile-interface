// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sessiontheme.h"

#include <KConfig>
#include <KConfigGroup>
#include <KPackage/PackageLoader>

#include <QColor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

using namespace Qt::Literals::StringLiterals;

namespace {
// Group paths use the file's own header form, "Colors:Header][Inactive" for nested groups.
using FileValues = QMap<QString, QMap<QString, QString>>;
using ThemeValues = QMap<QString, FileValues>;

const QString GENERAL_GROUP = u"General"_s;
const QString DARK_MODE_KEY = u"darkMode"_s;
const QString GROUP_SEPARATOR = u"]["_s;
const QString KDEGLOBALS = u"kdeglobals"_s;

QString colorSchemePath(const QString &name)
{
    return name.isEmpty() ? QString() : QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"color-schemes/"_s + name + u".colors"_s);
}

bool desktopIsDark()
{
    // SimpleConfig reads only the named file, never the session overlay. A global theme's
    // own values live in kdedefaults, which startplasma only adds to the path later.
    QString scheme;
    for (const QString &file : QStringList{KDEGLOBALS, u"kdedefaults/"_s + KDEGLOBALS}) {
        scheme = KConfig(file, KConfig::SimpleConfig).group(GENERAL_GROUP).readEntry("ColorScheme", QString());
        if (!scheme.isEmpty()) {
            break;
        }
    }
    const QString schemePath = colorSchemePath(scheme);
    if (schemePath.isEmpty()) {
        return false;
    }
    const KConfig colors(schemePath, KConfig::SimpleConfig);
    const QColor background = colors.group(u"Colors:Window"_s).readEntry("BackgroundNormal", QColor(Qt::white));
    return qGray(background.rgb()) < 128;
}

void addGroup(FileValues &values, const KConfigGroup &group, const QString &path)
{
    const auto entries = group.entryMap();
    for (auto it = entries.cbegin(); it != entries.cend(); ++it) {
        if (!it.key().startsWith(u"Name"_s)) {
            values[path][it.key()] = it.value();
        }
    }
    const auto children = group.groupList();
    for (const QString &child : children) {
        addGroup(values, group.group(child), path + GROUP_SEPARATOR + child);
    }
}

ThemeValues themeValues(const QString &packageId)
{
    const auto package = KPackage::PackageLoader::self()->loadPackage(u"Plasma/LookAndFeel"_s, packageId);
    const QString defaultsPath = package.isValid() ? package.filePath("defaults") : QString();
    if (defaultsPath.isEmpty()) {
        qWarning() << "Global theme" << packageId << "is missing; the session theme is left as it was";
        return {};
    }

    ThemeValues values;
    const KConfig defaults(defaultsPath, KConfig::SimpleConfig);
    const auto files = defaults.groupList();
    for (const QString &file : files) {
        if (file == u"Wallpaper") { // the shell sets its own wallpaper
            continue;
        }
        const KConfigGroup fileGroup = defaults.group(file);
        const auto groups = fileGroup.groupList();
        for (const QString &group : groups) {
            addGroup(values[file], fileGroup.group(group), group);
        }
    }
    values[KDEGLOBALS][u"KDE"_s][u"LookAndFeelPackage"_s] = packageId;

    // Colours go into the overlay as well, so they don't come from the desktop's kdeglobals.
    const QString schemePath = colorSchemePath(values[KDEGLOBALS][GENERAL_GROUP].value(u"ColorScheme"_s));
    if (!schemePath.isEmpty()) {
        const KConfig colors(schemePath, KConfig::SimpleConfig);
        const auto groups = colors.groupList();
        for (const QString &group : groups) {
            addGroup(values[KDEGLOBALS], colors.group(group), group);
        }
    }
    return values;
}

// Adds or removes [$i] on the given keys only; every other line stays as it is.
void setImmutable(const QString &path, const FileValues &keys, bool immutable)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QStringList lines;
    QString group;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        const qsizetype equals = line.indexOf(u'=');
        if (equals < 0) {
            if (line.startsWith(u'[') && line.endsWith(u']')) {
                group = line.mid(1, line.length() - 2);
            }
            lines << line;
            continue;
        }
        QString key = line.left(equals);
        if (key.endsWith(u"[$i]"_s)) {
            key.chop(4);
        }
        if (keys.value(group).contains(key)) {
            line = key + (immutable ? u"[$i]"_s : QString()) + line.mid(equals);
        }
        lines << line;
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Unable to write" << path;
        return;
    }
    QTextStream out(&file);
    for (const QString &line : std::as_const(lines)) {
        out << line << '\n';
    }
}

KConfigGroup nestedGroup(KConfig &config, const QString &path)
{
    const auto names = path.split(GROUP_SEPARATOR);
    KConfigGroup group = config.group(names.first());
    for (qsizetype i = 1; i < names.size(); ++i) {
        group = group.group(names.at(i));
    }
    return group;
}

QString dbusPath(const QString &file)
{
    QString path = u"/"_s + file;
    for (QChar &c : path) {
        if (c.unicode() >= 128 || (!c.isLetterOrNumber() && c != u'_' && c != u'/')) {
            c = u'_';
        }
    }
    return path;
}

// The overlay files are written by path, so KConfig's own notification names the wrong
// file; send what a change to the real file would send.
void notifyRunningApps(const ThemeValues &values)
{
    qDBusRegisterMetaType<QByteArrayList>();
    qDBusRegisterMetaType<QHash<QString, QByteArrayList>>();

    for (auto file = values.cbegin(); file != values.cend(); ++file) {
        QHash<QString, QByteArrayList> changes;
        for (auto group = file->cbegin(); group != file->cend(); ++group) {
            QString name = group.key();
            name.replace(GROUP_SEPARATOR, u"\x1d"_s);
            for (auto key = group->cbegin(); key != group->cend(); ++key) {
                changes[name] << key.key().toUtf8();
            }
        }
        QDBusMessage message = QDBusMessage::createSignal(dbusPath(file.key()), u"org.kde.kconfig.notify"_s, u"ConfigChanged"_s);
        message.setArguments({QVariant::fromValue(changes)});
        QDBusConnection::sessionBus().send(message);
    }

    // Running apps reload colours, style and cursor on these (KGlobalSettings change types).
    for (int type : {0, 2, 5}) {
        QDBusMessage change = QDBusMessage::createSignal(u"/KGlobalSettings"_s, u"org.kde.KGlobalSettings"_s, u"notifyChange"_s);
        change.setArguments({type, 0});
        QDBusConnection::sessionBus().send(change);
    }
    QDBusMessage icons = QDBusMessage::createSignal(u"/KIconLoader"_s, u"org.kde.KIconLoader"_s, u"iconChanged"_s);
    icons.setArguments({0});
    QDBusConnection::sessionBus().send(icons);
    QDBusConnection::sessionBus().send(QDBusMessage::createSignal(u"/KWin"_s, u"org.kde.KWin"_s, u"reconfigure"_s));
}
}

namespace SessionTheme {
void apply(const KSharedConfig::Ptr &sessionConfig, const QString &overlayDir, const QString &lightPackage, const QString &darkPackage)
{
    KConfigGroup general(sessionConfig, GENERAL_GROUP);
    if (!general.hasKey(DARK_MODE_KEY)) {
        general.writeEntry(DARK_MODE_KEY, desktopIsDark(), KConfigGroup::Notify);
        sessionConfig->sync();
    }
    const bool dark = general.readEntry(DARK_MODE_KEY, false);
    const QString packageId = dark ? darkPackage : lightPackage;

    const ThemeValues chosen = themeValues(packageId);
    if (chosen.isEmpty()) {
        return;
    }
    // Everything either variant sets, so switching leaves nothing of the other behind.
    ThemeValues all = themeValues(dark ? lightPackage : darkPackage);
    for (auto file = chosen.cbegin(); file != chosen.cend(); ++file) {
        for (auto group = file->cbegin(); group != file->cend(); ++group) {
            all[file.key()][group.key()].insert(group.value());
        }
    }

    const QDir configDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
    const QString kdeglobalsPath = configDir.filePath(overlayDir + u'/' + KDEGLOBALS);
    const QString previous = KConfig(kdeglobalsPath, KConfig::SimpleConfig).group(u"KDE"_s).readEntry("LookAndFeelPackage", QString());

    for (auto file = all.cbegin(); file != all.cend(); ++file) {
        const QString path = configDir.filePath(overlayDir + u'/' + file.key());
        QDir().mkpath(QFileInfo(path).absolutePath());
        setImmutable(path, file.value(), false);

        KConfig config(path, KConfig::SimpleConfig);
        const FileValues wanted = chosen.value(file.key());
        for (auto group = file->cbegin(); group != file->cend(); ++group) {
            KConfigGroup configGroup = nestedGroup(config, group.key());
            for (auto key = group->cbegin(); key != group->cend(); ++key) {
                if (wanted.value(group.key()).contains(key.key())) {
                    configGroup.writeEntry(key.key(), wanted.value(group.key()).value(key.key()));
                } else {
                    configGroup.deleteEntry(key.key());
                }
            }
        }
        config.sync();
        setImmutable(path, wanted, true);
    }

    if (previous != packageId) {
        notifyRunningApps(chosen);
    }
}
}

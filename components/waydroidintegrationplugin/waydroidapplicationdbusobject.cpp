/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidapplicationdbusobject.h"
#include "waydroidapplicationadaptor.h"
#include "waydroidintegrationplugin_debug.h"

#include <QDBusConnection>
#include <QLoggingCategory>
#include <QRegularExpression>

using namespace Qt::StringLiterals;

static const QRegularExpression nameRegExp(u"^Name:\\s*(\\S+)"_s);
static const QRegularExpression packageNameRegExp(u"^packageName:\\s*(\\S+)"_s);

WaydroidApplicationDBusObject::WaydroidApplicationDBusObject(const QString &name, const QString &packageName, QObject *parent)
    : QObject{parent}
    , m_name{name}
    , m_packageName{packageName}
{
}

void WaydroidApplicationDBusObject::registerObject()
{
    if (!m_dbusInitialized) {
        new WaydroidApplicationAdaptor{this};
        const QString objectPath = u"/Waydroid/Application/%1"_s.arg(m_packageName);
        QDBusConnection::sessionBus().registerObject(objectPath, this);
        m_dbusInitialized = true;
    }
}

void WaydroidApplicationDBusObject::unregisterObject()
{
    if (m_dbusInitialized) {
        const QString objectPath = u"/Waydroid/Application/%1"_s.arg(m_packageName);
        QDBusConnection::sessionBus().unregisterObject(objectPath);
        m_dbusInitialized = false;
    }
}

std::optional<WaydroidApplicationDBusObject> WaydroidApplicationDBusObject::parseApplicationFromWaydroidLog(QTextStream &inFile)
{
    const QString line = inFile.readLine();
    const QRegularExpressionMatch nameMatch = nameRegExp.match(line);

    if (!nameMatch.hasMatch() || nameMatch.lastCapturedIndex() == 0) {
        return std::nullopt;
    }

    QString name = nameMatch.captured(nameMatch.lastCapturedIndex());
    QString packageName;

    qint64 oldPos = inFile.pos();
    while (!inFile.atEnd()) {
        const QString line = inFile.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const QRegularExpressionMatch nameMatch = nameRegExp.match(line);
        if (nameMatch.hasMatch()) {
            inFile.seek(oldPos); // Revert file cursor position for the next Application parsing
            break;
        }

        const QRegularExpressionMatch packageNameMatch = packageNameRegExp.match(line);
        if (packageNameMatch.hasMatch() && packageNameMatch.lastCapturedIndex() > 0) {
            packageName = packageNameMatch.captured(packageNameMatch.lastCapturedIndex());
        }

        oldPos = inFile.pos();
    }

    if (packageName.isEmpty()) {
        return std::nullopt;
    }

    return WaydroidApplicationDBusObject(name, packageName);
}

QList<WaydroidApplicationDBusObject> WaydroidApplicationDBusObject::parseApplicationsFromWaydroidLog(QTextStream &inFile)
{
    QList<WaydroidApplicationDBusObject> applications;
    while (!inFile.atEnd()) {
        std::optional<WaydroidApplicationDBusObject> appOpt = parseApplicationFromWaydroidLog(inFile);
        if (!appOpt.has_value()) {
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to fetch the application: Maybe wrong QTextStream cursor position.";
            break;
        }

        const WaydroidApplicationDBusObject &app = appOpt.value();
        qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Waydroid application found: " << app.name() << " (" << app.packageName() << ")";
        applications.append(app);
    }
    return applications;
}

QString WaydroidApplicationDBusObject::name() const
{
    return m_name;
}

QString WaydroidApplicationDBusObject::packageName() const
{
    return m_packageName;
}
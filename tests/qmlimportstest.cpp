// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QLibraryInfo>
#include <QMap>
#include <QRegularExpression>
#include <QSet>
#include <QTest>

// Every QML module the repository imports must be installed or defined by the repository.
// One missing module stops a whole QML library loading, and nothing else reports it before run time.
class QmlImportsTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void testImportsResolve()
    {
        const QDir root(QStringLiteral(SOURCE_ROOT));
        QVERIFY(root.exists());

        const QSet<QString> defined = modulesDefinedInRepository(root);
        QStringList importPaths{QLibraryInfo::path(QLibraryInfo::QmlImportsPath)};
        importPaths += qEnvironmentVariable("QML_IMPORT_PATH").split(QLatin1Char(':'), Qt::SkipEmptyParts);

        // Registered by the process that loads the QML, with no qmldir to find.
        const QSet<QString> registeredInProcess{
            QStringLiteral("initialstart"),
            QStringLiteral("org.kde.kwin"),
            QStringLiteral("org.kde.timesettings"),
            QStringLiteral("org.sonicde.sonicshell.panel"),
        };

        static const QRegularExpression importLine(QStringLiteral("^\\s*import\\s+([A-Za-z_][\\w.]*)"), QRegularExpression::MultilineOption);

        QMap<QString, QString> missing;
        int files = 0;
        QDirIterator it(root.path(), {QStringLiteral("*.qml")}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString path = it.next();
            if (path.contains(QLatin1String("/graphify-out/"))) {
                continue;
            }
            QFile file(path);
            QVERIFY(file.open(QIODevice::ReadOnly));
            ++files;

            auto matches = importLine.globalMatch(QString::fromUtf8(file.readAll()));
            while (matches.hasNext()) {
                const QString module = matches.next().captured(1);
                if (defined.contains(module) || registeredInProcess.contains(module) || module.startsWith(QLatin1String("plasma.applet."))) {
                    continue;
                }
                const QString relative = QString(module).replace(QLatin1Char('.'), QLatin1Char('/')) + QStringLiteral("/qmldir");
                const bool found = std::any_of(importPaths.cbegin(), importPaths.cend(), [&relative](const QString &importPath) {
                    return QFile::exists(importPath + QLatin1Char('/') + relative);
                });
                if (!found) {
                    missing.insert(module, root.relativeFilePath(path));
                }
            }
        }

        QVERIFY(files > 0);

        QString message;
        for (auto entry = missing.cbegin(); entry != missing.cend(); ++entry) {
            message += QStringLiteral("\n%1 (imported by %2)").arg(entry.key(), entry.value());
        }
        QVERIFY2(missing.isEmpty(), qPrintable(QStringLiteral("QML modules that can't be found:") + message));
    }

private:
    static QSet<QString> modulesDefinedInRepository(const QDir &root)
    {
        static const QRegularExpression uri(QStringLiteral("\\bURI\\s+([\\w.]+)"));

        QSet<QString> modules;
        QDirIterator it(root.path(), {QStringLiteral("CMakeLists.txt")}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QFile file(it.next());
            if (!file.open(QIODevice::ReadOnly)) {
                continue;
            }
            auto matches = uri.globalMatch(QString::fromUtf8(file.readAll()));
            while (matches.hasNext()) {
                modules.insert(matches.next().captured(1));
            }
        }
        return modules;
    }
};

QTEST_GUILESS_MAIN(QmlImportsTest)

#include "qmlimportstest.moc"

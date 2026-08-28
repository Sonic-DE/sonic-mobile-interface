// SPDX-License-Identifier: GPL-2.0-or-later

#include <KApplicationTrader>
#include <KConfigGroup>
#include <KLocalizedQmlContext>
#include <KPluginFactory>
#include <KSharedConfig>
#include <Plasma/Containment>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QtQuickTest>

class FolioAppDrawerSetup : public QObject {
    Q_OBJECT

public Q_SLOTS:
    void applicationAvailable()
    {
        QStandardPaths::setTestModeEnabled(true);
    }

    void qmlEngineAvailable(QQmlEngine *engine)
    {
        auto *localizedContext = new KLocalizedQmlContext(engine);
        localizedContext->setTranslationDomain(QStringLiteral("plasma_applet_org.kde.plasma.mobile.homescreen.folio"));
        engine->rootContext()->setContextObject(localizedContext);
        QQmlEngine::setContextForObject(localizedContext, engine->rootContext());
        const auto result = KPluginFactory::instantiatePlugin<Plasma::Containment>(KPluginMetaData(QStringLiteral(FOLIO_PLUGIN_PATH)));
        if (!result) {
            qFatal("Cannot load Folio test plugin: %s", qPrintable(result.errorString));
        }
        m_folio.reset(result.plugin);
        engine->rootContext()->setContextProperty(QStringLiteral("testFolio"), m_folio.get());

        const auto config = KSharedConfig::openConfig(QStringLiteral("applications-blacklistrc"));
        const auto blacklist = KConfigGroup(config, QStringLiteral("Applications")).readEntry("blacklist", QStringList());
        const auto apps = KApplicationTrader::query([&blacklist](const KService::Ptr &service) {
            return !service->noDisplay() && service->showOnCurrentPlatform() && !blacklist.contains(service->desktopEntryName());
        });
        QStringList storageIds;
        for (const auto &app : apps) {
            storageIds.append(app->storageId());
        }
        engine->rootContext()->setContextProperty(QStringLiteral("expectedStorageIds"), storageIds);
    }

private:
    std::unique_ptr<Plasma::Containment> m_folio;
};

QUICK_TEST_MAIN_WITH_SETUP(folioappdrawer, FolioAppDrawerSetup)

#include "folioappdrawertest.moc"

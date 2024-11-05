/*
 *  SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "shellsettingsplugin.h"

#include <KIO/CommandLauncherJob>
#include <KNotificationJobUiDelegate>
#include <KPluginFactory>
#include <KRuntimePlatform>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDebug>
#include "mobileshellsettings.h"

ShellSettingsPlugin::ShellSettingsPlugin(QObject *parent)
    : QObject{parent}
{

}

void ShellSettingsPlugin::updateNavigationBarsInPlasma(bool navigationPanelEnabled)
{
    auto config = MobileShellSettings::self();
    qDebug() << config->navigationPanelEnabled();
    // Do not update panels when not in Plasma Mobile
    bool isMobilePlatform = KRuntimePlatform::runtimePlatform().contains("phone");
    if (!isMobilePlatform) {
        return;
    }

    auto message = QDBusMessage::createMethodCall(QLatin1String("org.kde.plasmashell"),
                                                  QLatin1String("/PlasmaShell"),
                                                  QLatin1String("org.kde.PlasmaShell"),
                                                  QLatin1String("evaluateScript"));

    if (navigationPanelEnabled) {
        QString createNavigationPanelScript = R"(
            loadTemplate("org.kde.plasma.mobile.defaultNavigationPanel");
        )";

        message << createNavigationPanelScript;

    } else {
        QString deleteNavigationPanelScript = R"(
            let allPanels = panels();
            for (var i = 0; i < allPanels.length; i++) {
                if (allPanels[i].type === "org.kde.plasma.mobile.taskpanel") {
                    allPanels[i].remove();
                }
            }
        )";

        message << deleteNavigationPanelScript;
    }

    // TODO check for error response
    QDBusConnection::sessionBus().asyncCall(message);
}

// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>

#include <KAboutData>
#include <KLocalizedString>

#include "settings.h"
#include "version.h"
#include "wizard.h"

int main(int argc, char *argv[])
{
    // some ideas about how this can work:
    //
    // kded module:
    // Start Plasma
    // -> If desktop, check the configuration and popup a window if certain settings would be bad for it and should be changed
    // - maybe store whether the last session logged in was plasma mobile, and what settings we changed (so that we can revert immediately)

    // initialstart:
    // launched by kded
    // read config file provided by distro on what they want
    // -> Check if new install or major Plasma upgrade, if it is:
    //   -> Prompt user whether to change system settings during initial install process
    // -> Otherwise:
    //   -> If mobile, check the configuration and popup a window if we need certain settings to be set (replace plasma-phone-settings)
    // uses lockscreen overlay to be shown

    // settings:
    // -> set look and feel https://invent.kde.org/plasma/plasma-workspace/-/blob/master/kcms/lookandfeel/lnftool.cpp
    // -> set autologin and kscrdenlocker lock?
    // -> set blocklisted apps
    // -> set kwin maximized mode/window decorations (in the future just enable script)
    // -> set default apps (ex. angelfish, qmlkonsole)
    // -> set virtual keyboard
    // -> set blur

    QApplication app(argc, argv);

    // apply configuration
    Settings::self()->applyConfiguration();

    // if the wizard has already been run, or we aren't in plasma mobile
    if (!Settings::self()->shouldStartWizard()) {
        qDebug() << "Wizard will not be started since either it has already been run, or the current session is not Plasma Mobile.";
        return 0;
    }

    // start wizard
    KLocalizedString::setApplicationDomain("plasma-mobile-initial-start");
    KAboutData aboutData(QStringLiteral("plasma-mobile-initial-start"),
                         QStringLiteral("Initial Start"),
                         QStringLiteral(PLASMA_MOBILE_VERSION_STRING),
                         QStringLiteral(""),
                         KAboutLicense::GPL,
                         i18n("© 2023 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("devin@kde.org"));
    KAboutData::setApplicationData(aboutData);

    Wizard *wizard = new Wizard;
    qmlRegisterSingletonType<Wizard>("initialstart", 1, 0, "Wizard", [wizard](QQmlEngine *, QJSEngine *) -> QObject * {
        return wizard;
    });

    QQmlApplicationEngine *engine = new QQmlApplicationEngine;

    engine->rootContext()->setContextObject(new KLocalizedContext{engine});
    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("start-here-symbolic")));

    return app.exec();
}

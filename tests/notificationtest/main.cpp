// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>

#include <KAboutData>
#include <KLocalizedString>

#include "tests.h"
#include "utils.h"
#include "version.h"

using namespace Qt::Literals::StringLiterals;

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(u"list"_s, u"Lists the possible test notifications that can be set."_s));
    parser->addVersionOption();
    parser->addHelpOption();
    parser->addPositionalArgument("test", "The test notification to send.");
    return parser;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QScopedPointer<QCommandLineParser> parser{createParser()};
    parser->process(app);

    KLocalizedString::setApplicationDomain("plasma-mobile-notificationtest");
    QCoreApplication::setApplicationName(u"plasma-mobile-notificationtest"_s);
    QCoreApplication::setApplicationVersion(QStringLiteral(PLASMA_MOBILE_VERSION_STRING));
    QCoreApplication::setOrganizationDomain(u"kde.org"_s);

    QList<NotificationTest *> notificationTests = {new BasicNotificationTest,
                                                   new UrlNotificationTest,
                                                   new ReplyNotificationTest,
                                                   new LowUrgencyNotificationTest,
                                                   new HighUrgencyNotificationTest,
                                                   new CriticalUrgencyNotificationTest};

    if (parser->isSet(u"list"_s)) {
        for (auto notification : notificationTests) {
            qInfo() << notification->name();
        }
        return 0;
    } else if (parser->positionalArguments().size() <= 0) {
        parser->showHelp();
        return 0;
    }

    auto args = parser->positionalArguments();
    QString name = args[0];

    bool found = false;
    for (auto notification : notificationTests) {
        if (notification->name() == name) {
            qInfo() << "Sending notification" << notification->name();
            notification->sendNotification(app);
            found = true;
            break;
        }
    }

    if (!found) {
        qInfo() << "Test not found.";
    }

    return app.exec();
}

// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <KJob>
#include <KNotification>
#include <KNotificationReplyAction>

#include "tests.h"

NotificationTest::NotificationTest(QObject *parent)
    : QObject{parent}
{
}

void BasicNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setIconName(QStringLiteral("notification-active"));
    notification->setText("This is a test notification!");
    auto action = notification->addAction("Action!");
    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

void UrlNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setTitle("Web link");
    notification->setText("I like links!");
    notification->setUrls({QUrl{"https://kde.org/reusable-assets/home-blur.jpg"}});
    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

void ReplyNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setIconName(QStringLiteral("avatar-default-symbolic"));
    notification->setTitle("John");
    notification->setText("This is great news! Let's meet up tomorrow!");

    auto replyAction = std::make_unique<KNotificationReplyAction>("Reply");
    replyAction->setPlaceholderText("Reply to John...");
    QObject::connect(replyAction.get(), &KNotificationReplyAction::replied, [](const QString &text) {
        qDebug() << "you replied with" << text;
    });
    notification->setReplyAction(std::move(replyAction));

    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

void LowUrgencyNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setIconName(QStringLiteral("notification-inactive"));
    notification->setTitle("Low Urgency Notification");
    notification->setText("This is not very important...");
    notification->setUrgency(KNotification::CriticalUrgency);

    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

void HighUrgencyNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setIconName(QStringLiteral("notification-active"));
    notification->setTitle("Urgent Notification");
    notification->setText("This is very urgent! AAAAAA");
    notification->setUrgency(KNotification::CriticalUrgency);

    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

void CriticalUrgencyNotificationTest::sendNotification(QCoreApplication &app)
{
    KNotification *notification = new KNotification(QStringLiteral("notificationTest"));
    notification->setComponentName(QStringLiteral("plasma_mobile_notificationtest"));
    notification->setIconName(QStringLiteral("notification-active"));
    notification->setTitle("Critically Urgent Notification");
    notification->setText("This is very urgent! AAAAAA");
    notification->setUrgency(KNotification::CriticalUrgency);
    auto action = notification->addAction("Action!");
    notification->sendEvent();

    connect(notification, &KNotification::closed, &app, QCoreApplication::quit);
}

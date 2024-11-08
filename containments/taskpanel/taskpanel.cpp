/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "taskpanel.h"

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDebug>
#include <QGuiApplication>

#include <kscreen/configmonitor.h>
#include <kscreen/getconfigoperation.h>
#include <kscreen/output.h>
#include <kscreen/setconfigoperation.h>

// register type for Keyboards.KWinVirtualKeyboard.forceActivate();
Q_DECLARE_METATYPE(QDBusPendingReply<>)

KScreen::Output::Rotation mapReadingOrientation(QOrientationReading::Orientation orientation)
{
    switch (orientation) {
    case QOrientationReading::Orientation::TopUp:
        return KScreen::Output::Rotation::None;
    case QOrientationReading::Orientation::TopDown:
        return KScreen::Output::Rotation::Inverted;
    case QOrientationReading::Orientation::LeftUp:
        return KScreen::Output::Rotation::Right;
    case QOrientationReading::Orientation::RightUp:
        return KScreen::Output::Rotation::Left;
    case QOrientationReading::Orientation::FaceUp:
    case QOrientationReading::Orientation::FaceDown:
    case QOrientationReading::Orientation::Undefined:
        return KScreen::Output::Rotation::None;
    }
    return KScreen::Output::Rotation::None;
}

TaskPanel::TaskPanel(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : Plasma::Containment(parent, data, args)
    , m_sensor{new QOrientationSensor(this)}
{
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished, this, [this](auto *op) {
        m_config = qobject_cast<KScreen::GetConfigOperation *>(op)->config();
        KScreen::ConfigMonitor::instance()->addConfig(m_config);
    });

    connect(m_sensor, &QOrientationSensor::readingChanged, this, &TaskPanel::updateShowRotationButton);
}

void TaskPanel::triggerTaskSwitcher() const
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.kglobalaccel", "/component/kwin", "org.kde.kglobalaccel.Component", "invokeShortcut");
    message.setArguments({QStringLiteral("Mobile Task Switcher")});

    // this does not block, so it won't necessarily be called before the method returns
    QDBusConnection::sessionBus().send(message);
}

void TaskPanel::rotateToSuggestedRotation()
{
    const auto outputs = m_config->outputs();
    if (outputs.empty()) {
        return;
    }

    // HACK: Assume the output we care about is the first device
    const auto output = outputs[0];
    output->setRotation(m_rotateTo);

    auto setop = new KScreen::SetConfigOperation(m_config, this);
    setop->exec();
}

bool TaskPanel::showRotationButton() const
{
    return m_showRotationButton;
}

void TaskPanel::updateShowRotationButton()
{
    QOrientationReading *reading = m_sensor->reading();
    m_rotateTo = mapReadingOrientation(reading->orientation());

    const auto outputs = m_config->outputs();

    if (outputs.empty()) {
        m_showRotationButton = false;
        Q_EMIT showRotationButtonChanged();
        return;
    }

    // HACK: Assume the output we care about is the first device
    const auto output = outputs[0];
    m_showRotationButton = output->rotation() != m_rotateTo;
    Q_EMIT showRotationButtonChanged();
}

K_PLUGIN_CLASS(TaskPanel)

#include "taskpanel.moc"

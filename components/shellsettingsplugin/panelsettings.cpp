// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "panelsettings.h"

#include <kscreen/configmonitor.h>
#include <kscreen/getconfigoperation.h>
#include <kscreen/output.h>

using namespace Qt::Literals::StringLiterals;

const QString CONFIG_FILE = u"plasmamobilerc"_s;
const QString PANELS_CONFIG_GROUP = u"Panels"_s;

// Orientations
const QString TOP_CONFIG_GROUP = u"WhenOnTop"_s;
const QString LEFT_CONFIG_GROUP = u"WhenOnLeft"_s;
const QString RIGHT_CONFIG_GROUP = u"WhenOnRight"_s;
const QString BOTTOM_CONFIG_GROUP = u"WhenOnBottom"_s;

QString mapRotationToTopPosition(KScreen::Output::Rotation rotation)
{
    switch (rotation) {
    case KScreen::Output::Rotation::Left:
        return RIGHT_CONFIG_GROUP;
    case KScreen::Output::Rotation::Inverted:
        return BOTTOM_CONFIG_GROUP;
    case KScreen::Output::Rotation::Right:
        return LEFT_CONFIG_GROUP;
    default:
        return TOP_CONFIG_GROUP;
    }
}

QString mapRotationToBottomPosition(KScreen::Output::Rotation rotation)
{
    switch (rotation) {
    case KScreen::Output::Rotation::Left:
        return LEFT_CONFIG_GROUP;
    case KScreen::Output::Rotation::Inverted:
        return TOP_CONFIG_GROUP;
    case KScreen::Output::Rotation::Right:
        return RIGHT_CONFIG_GROUP;
    default:
        return BOTTOM_CONFIG_GROUP;
    }
}

PanelSettings::PanelSettings(QObject *parent)
    : QObject{parent}
    , m_config{KSharedConfig::openConfig(CONFIG_FILE, KConfig::SimpleConfig)}
{
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished, this, [this](auto *op) {
        m_kscreenConfig = qobject_cast<KScreen::GetConfigOperation *>(op)->config();

        if (!m_kscreenConfig) {
            return;
        }

        KScreen::ConfigMonitor::instance()->addConfig(m_kscreenConfig);

        // update all screens with event connect
        for (KScreen::OutputPtr output : m_kscreenConfig->outputs()) {
            connect(output.data(), &KScreen::Output::rotationChanged, this, &PanelSettings::updateFields);
            connect(output.data(), &KScreen::Output::scaleChanged, this, &PanelSettings::updateFields);
        }

        // listen to all new screens and connect
        connect(m_kscreenConfig.data(), &KScreen::Config::outputAdded, this, [this](const auto &output) {
            connect(output.data(), &KScreen::Output::rotationChanged, this, &PanelSettings::updateFields);
            connect(output.data(), &KScreen::Output::scaleChanged, this, &PanelSettings::updateFields);
        });

        updateFields();
    });
}

void PanelSettings::updateFields()
{
    if (!m_kscreenConfig) {
        return;
    }

    const auto outputs = m_kscreenConfig->outputs();
    if (outputs.empty()) {
        return;
    }

    for (KScreen::OutputPtr output : outputs) {
        // apparently it's possible to get nullptr outputs?
        if (!output || output->id() != m_screenId) {
            continue;
        }

        auto group = KConfigGroup{m_config, PANELS_CONFIG_GROUP};
        auto topGroup = KConfigGroup{&group, mapRotationToTopPosition(output->rotation())};
        auto bottomGroup = KConfigGroup{&group, mapRotationToBottomPosition(output->rotation())};

        // Divide values by the display's scale for scaling independent sizing
        setStatusBarHeight(topGroup.readEntry(u"statusBarHeight"_s, -1.0) / output->scale()); // negative to let Kirigami auto calculate
        setStatusBarLeftPadding(topGroup.readEntry(u"statusBarLeftPadding"_s, 0.0) / output->scale());
        setStatusBarRightPadding(topGroup.readEntry(u"statusBarRightPadding"_s, 0.0) / output->scale());
        setStatusBarCenterSpacing(topGroup.readEntry(u"statusBarCenterSpacing"_s, 0.0) / output->scale());
        setNavigationPanelHeight(bottomGroup.readEntry(u"navigationPanelHeight"_s, -1.0) / output->scale()); // negative to let Kirigami auto calculate
        setNavigationPanelLeftPadding(bottomGroup.readEntry(u"navigationPanelLeftPadding"_s, 0.0) / output->scale());
        setNavigationPanelRightPadding(bottomGroup.readEntry(u"navigationPanelRightPadding"_s, 0.0) / output->scale());

        return;
    }
}

int PanelSettings::screenId() const
{
    return m_screenId;
}

void PanelSettings::setScreenId(int screen)
{
    if (screen == m_screenId) {
        return;
    }

    m_screenId = screen;
    Q_EMIT screenIdChanged();

    updateFields();
}

qreal PanelSettings::statusBarHeight() const
{
    return m_statusBarHeight;
}

void PanelSettings::setStatusBarHeight(qreal statusBarHeight)
{
    if (statusBarHeight == m_statusBarHeight) {
        return;
    }
    m_statusBarHeight = statusBarHeight;
    Q_EMIT statusBarHeightChanged();
}

qreal PanelSettings::statusBarLeftPadding() const
{
    return m_statusBarLeftPadding;
}

void PanelSettings::setStatusBarLeftPadding(qreal statusBarLeftPadding)
{
    if (statusBarLeftPadding == m_statusBarLeftPadding) {
        return;
    }
    m_statusBarLeftPadding = statusBarLeftPadding;
    Q_EMIT statusBarLeftPaddingChanged();
}

qreal PanelSettings::statusBarRightPadding() const
{
    return m_statusBarRightPadding;
}

void PanelSettings::setStatusBarRightPadding(qreal statusBarRightPadding)
{
    if (statusBarRightPadding == m_statusBarRightPadding) {
        return;
    }
    m_statusBarRightPadding = statusBarRightPadding;
    Q_EMIT statusBarRightPaddingChanged();
}

qreal PanelSettings::statusBarCenterSpacing() const
{
    return m_statusBarCenterSpacing;
}

void PanelSettings::setStatusBarCenterSpacing(qreal statusBarCenterSpacing)
{
    if (statusBarCenterSpacing == m_statusBarCenterSpacing) {
        return;
    }
    m_statusBarCenterSpacing = statusBarCenterSpacing;
    Q_EMIT statusBarCenterSpacingChanged();
}

qreal PanelSettings::navigationPanelHeight() const
{
    return m_navigationPanelHeight;
}

void PanelSettings::setNavigationPanelHeight(qreal navigationPanelHeight)
{
    if (navigationPanelHeight == m_navigationPanelHeight) {
        return;
    }
    m_navigationPanelHeight = navigationPanelHeight;
    Q_EMIT navigationPanelHeightChanged();
}

qreal PanelSettings::navigationPanelLeftPadding() const
{
    return m_navigationPanelLeftPadding;
}

void PanelSettings::setNavigationPanelLeftPadding(qreal navigationPanelLeftPadding)
{
    if (navigationPanelLeftPadding == m_navigationPanelLeftPadding) {
        return;
    }
    m_navigationPanelLeftPadding = navigationPanelLeftPadding;
    Q_EMIT navigationPanelLeftPaddingChanged();
}

qreal PanelSettings::navigationPanelRightPadding() const
{
    return m_navigationPanelRightPadding;
}

void PanelSettings::setNavigationPanelRightPadding(qreal navigationPanelRightPadding)
{
    if (navigationPanelRightPadding == m_navigationPanelRightPadding) {
        return;
    }
    m_navigationPanelRightPadding = navigationPanelRightPadding;
    Q_EMIT navigationPanelRightPaddingChanged();
}

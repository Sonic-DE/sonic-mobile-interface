// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include <KConfigGroup>
#include <KConfigWatcher>
#include <KSharedConfig>

#include <kscreen/config.h>
#include <qqmlregistration.h>

/**
 * This should only be applicable on the internal device display.
 *
 * TODO: eventually, we want to support configuring multiple displays (ex. foldable devices)
 */
class PanelSettings : public QObject
{
    Q_OBJECT
    // Client must set the screen they want to get details for
    Q_PROPERTY(int screenId READ screenId WRITE setScreenId NOTIFY screenIdChanged)

    Q_PROPERTY(qreal statusBarHeight READ statusBarHeight NOTIFY statusBarHeightChanged)
    Q_PROPERTY(qreal statusBarLeftPadding READ statusBarLeftPadding NOTIFY statusBarLeftPaddingChanged)
    Q_PROPERTY(qreal statusBarRightPadding READ statusBarRightPadding NOTIFY statusBarRightPaddingChanged)
    Q_PROPERTY(qreal statusBarCenterSpacing READ statusBarCenterSpacing NOTIFY statusBarCenterSpacingChanged)

    Q_PROPERTY(qreal navigationPanelHeight READ navigationPanelHeight NOTIFY navigationPanelHeightChanged)
    Q_PROPERTY(qreal navigationPanelLeftPadding READ navigationPanelLeftPadding NOTIFY navigationPanelLeftPaddingChanged)
    Q_PROPERTY(qreal navigationPanelRightPadding READ navigationPanelRightPadding NOTIFY navigationPanelRightPaddingChanged)

    QML_ELEMENT

public:
    explicit PanelSettings(QObject *parent = nullptr);

    int screenId() const;
    void setScreenId(int screen);

    qreal statusBarHeight() const;
    qreal statusBarLeftPadding() const;
    qreal statusBarRightPadding() const;
    qreal statusBarCenterSpacing() const;

    qreal navigationPanelHeight() const;
    qreal navigationPanelLeftPadding() const;
    qreal navigationPanelRightPadding() const;

Q_SIGNALS:
    void screenIdChanged();
    void statusBarHeightChanged();
    void statusBarLeftPaddingChanged();
    void statusBarRightPaddingChanged();
    void statusBarCenterSpacingChanged();
    void navigationPanelHeightChanged();
    void navigationPanelLeftPaddingChanged();
    void navigationPanelRightPaddingChanged();

private:
    void updateFields();

    void setStatusBarHeight(qreal statusBarHeight);
    void setStatusBarLeftPadding(qreal statusBarLeftPadding);
    void setStatusBarRightPadding(qreal statusBarRightPadding);
    void setStatusBarCenterSpacing(qreal statusBarCenterSpacing);

    void setNavigationPanelHeight(qreal navigationPanelHeight);
    void setNavigationPanelLeftPadding(qreal navigationPanelLeftPadding);
    void setNavigationPanelRightPadding(qreal navigationPanelRightPadding);

    int m_screenId = -1;

    qreal m_statusBarHeight = -1;
    qreal m_statusBarLeftPadding = 0;
    qreal m_statusBarRightPadding = 0;
    qreal m_statusBarCenterSpacing = 0;
    qreal m_navigationPanelHeight = -1;
    qreal m_navigationPanelLeftPadding = 0;
    qreal m_navigationPanelRightPadding = 0;

    KSharedConfig::Ptr m_config;
    KScreen::ConfigPtr m_kscreenConfig{nullptr};
};

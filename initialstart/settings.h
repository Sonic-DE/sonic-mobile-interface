// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include <KConfigGroup>
#include <KSharedConfig>

class Settings : public QObject
{
    Q_OBJECT

public:
    Settings(QObject *parent = nullptr);
    static Settings *self();

    // whether the initial start wizard should be started
    bool shouldStartWizard();

    // set that the wizard has finished
    void setWizardFinished();

    // apply the configuration
    void applyConfiguration();

private:
    bool m_wizardRun;

    // whether this is Plasma Mobile
    bool m_isMobilePlatform;

    KSharedConfig::Ptr m_initialStartConfig;
    KSharedConfig::Ptr m_kwinrcConfig;
    KSharedConfig::Ptr m_appBlacklistConfig;
    KSharedConfig::Ptr m_kdeglobalsConfig;
};

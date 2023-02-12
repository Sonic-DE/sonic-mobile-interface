// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "settings.h"
#include "config.h"

#include <KRuntimePlatform>

#include <QDebug>
#include <QProcess>

const QString CONFIG_FILE = QStringLiteral("plasmamobilerc");
const QString INITIAL_START_CONFIG_GROUP = QStringLiteral("InitialStart");

Settings::Settings(QObject *parent)
    : QObject{parent}
    , m_isMobilePlatform{KRuntimePlatform::runtimePlatform().contains(QStringLiteral("phone"))}
    , m_initialStartConfig{KSharedConfig::openConfig(CONFIG_FILE, KConfig::SimpleConfig)}
    , m_kwinrcConfig{KSharedConfig::openConfig(QStringLiteral("kwinrc"), KConfig::SimpleConfig)}
    , m_appBlacklistConfig{KSharedConfig::openConfig(QStringLiteral("applications-blacklistrc"), KConfig::SimpleConfig)}
    , m_kdeglobalsConfig{KSharedConfig::openConfig(QStringLiteral("kdeglobals"), KConfig::SimpleConfig)}
{
}

Settings *Settings::self()
{
    static Settings *settings = new Settings;
    return settings;
}

bool Settings::shouldStartWizard()
{
    if (!m_isMobilePlatform) {
        return false;
    }

    auto group = KConfigGroup{m_initialStartConfig, INITIAL_START_CONFIG_GROUP};
    return !group.readEntry("wizardRun", false);
}

void Settings::setWizardFinished()
{
    auto group = KConfigGroup{m_initialStartConfig, INITIAL_START_CONFIG_GROUP};
    group.writeEntry("wizardRun", true, KConfigGroup::Notify);
    m_initialStartConfig->sync();
}

void Settings::applyConfiguration()
{
    if (!m_isMobilePlatform) {
        qDebug() << "Configuration will not be applied, as the session is not Plasma Mobile.";
        return;
    }

    QProcess::execute("plasma-apply-lookandfeel", {"-a", "org.kde.plasma.phone"});

    // kwinrc
    for (auto groupName : KWINRC_SETTINGS.keys()) {
        auto group = KConfigGroup{m_kwinrcConfig, groupName};
        for (auto key : KWINRC_SETTINGS[groupName].keys()) {
            qDebug() << "In kwinrc, set" << key << "to" << KWINRC_SETTINGS[groupName][key];
            group.writeEntry(key, KWINRC_SETTINGS[groupName][key], KConfigGroup::Notify);
        }
    }
    m_kwinrcConfig->sync();

    // applications-blacklistrc
    // NOTE: we only write these entries if they are not already defined in the config
    for (auto groupName : APPLICATIONS_BLACKLIST_SETTINGS.keys()) {
        auto group = KConfigGroup{m_appBlacklistConfig, groupName};
        for (auto key : APPLICATIONS_BLACKLIST_SETTINGS[groupName].keys()) {
            if (!group.hasKey(key)) {
                qDebug() << "In applications-blacklistrc, set" << key << "to" << APPLICATIONS_BLACKLIST_SETTINGS[groupName][key];
                group.writeEntry(key, APPLICATIONS_BLACKLIST_SETTINGS[groupName][key], KConfigGroup::Notify);
            }
        }
    }
    m_appBlacklistConfig->sync();

    // kdeglobals
    // NOTE: we only write these entries if they are not already defined in the config
    for (auto groupName : KDEGLOBALS_SETTINGS.keys()) {
        auto group = KConfigGroup{m_kdeglobalsConfig, groupName};
        for (auto key : KDEGLOBALS_SETTINGS[groupName].keys()) {
            if (!group.hasKey(key)) {
                qDebug() << "In kdeglobals, set" << key << "to" << KDEGLOBALS_SETTINGS[groupName][key];
                group.writeEntry(key, KDEGLOBALS_SETTINGS[groupName][key], KConfigGroup::Notify);
            }
        }
    }
    m_kdeglobalsConfig->sync();
}

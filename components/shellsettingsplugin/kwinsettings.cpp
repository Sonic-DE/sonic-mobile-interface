/*
 *  SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kwinsettings.h"

const QString CONFIG_FILE = QStringLiteral("kwinrc");

KWinSettings::KWinSettings(QObject *parent)
    : QObject{parent}
    , m_config{KSharedConfig::openConfig(CONFIG_FILE)}
    , m_overlayConfig{KSharedConfig::openConfig(OVERLAY_CONFIG_FILE)}
{
    m_configWatcher = KConfigWatcher::create(m_config);
    connect(m_configWatcher.data(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group, const QByteArrayList &names) -> void {
        Q_UNUSED(names)
    });
}

bool KWinSettings::doubleTapWakeup() const
{
    return false;
}

void KWinSettings::setDoubleTapWakeup(bool enabled)
{
}

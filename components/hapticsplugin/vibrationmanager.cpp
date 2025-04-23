// SPDX-FileCopyrightText: 2023-2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "vibrationmanager.h"

VibrationManager::VibrationManager(QObject *parent)
    : QObject{parent}
{
    qDBusRegisterMetaType<VibrationEvent>();
    qDBusRegisterMetaType<VibrationEventList>();
}

void VibrationManager::vibrate(int durationMs)
{
    // Only create interface when needed.
    if (!m_interface) {
        const auto objectPath = QStringLiteral("/org/sigxcpu/Feedback");
        m_interface = new OrgSigxcpuFeedbackHapticInterface("org.sigxcpu.Feedback", objectPath, QDBusConnection::sessionBus(), this);
    }

    const QString appId = QStringLiteral("org.kde.plasmashell");
    const VibrationEvent event{1.0, static_cast<quint32>(durationMs)};
    const VibrationEventList pattern = {event};
    bool success = m_interface->Vibrate(appId, pattern);
    Q_UNUSED(success);
}

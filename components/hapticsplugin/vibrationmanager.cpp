// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "vibrationmanager.h"

VibrationManager::VibrationManager(QObject *parent)
    : QObject{parent}
{
}

void VibrationManager::vibrate(int durationMs)
{
    // Only create interface when needed.
    if (!m_interface) {
        const auto objectPath = QStringLiteral("/org/sigxcpu/Feedback");
        m_interface = new org::sigxcpu::Feedback::Haptic("org.sigxcpu.Feedback.Haptic", objectPath, QDBusConnection::systemBus(), this);
    }

    const QString appId = QStringLiteral("org.kde.plasma.mobileshell");
    const QVariant stamp = QVariant::fromValue(QPair<double, int>(1.0, durationMs));
    const QVariantList pattern = {stamp};
    bool success = m_interface->Vibrate(appId, pattern);
    Q_UNUSED(success);
}

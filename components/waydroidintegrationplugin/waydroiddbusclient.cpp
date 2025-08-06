/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroiddbusclient.h"

#include <QDBusServiceWatcher>

WaydroidDBusClient::WaydroidDBusClient(QObject *parent)
    : QObject{parent}
    , m_interface{new OrgKdePlasmashellInterface{QStringLiteral("org.kde.plasmashell"), QStringLiteral("/Waydroid"), QDBusConnection::sessionBus(), this}}
    , m_connected{false}
{
    // Check if the service is already running
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(QStringLiteral("org.kde.plasmashell"))) {
        m_connected = true;
        if (m_interface->isValid()) {
            connectSignals();
        }
    }

    connect(QDBusConnection::sessionBus().interface(),
            &QDBusConnectionInterface::serviceOwnerChanged,
            this,
            [this](const QString &service, const QString &oldOwner, const QString &newOwner) {
                Q_UNUSED(oldOwner);
                if (service == QStringLiteral("org.kde.plasmashell")) {
                    if (!newOwner.isEmpty() && !m_connected) {
                        m_connected = true;
                        if (m_interface->isValid()) {
                            connectSignals();
                        }
                    } else if (newOwner.isEmpty() && m_connected) {
                        m_connected = false;
                    }
                }
            });
}

void WaydroidDBusClient::connectSignals()
{
}
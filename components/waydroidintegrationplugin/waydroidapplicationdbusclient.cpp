/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidapplicationdbusclient.h"
#include "waydroidintegrationplugin_debug.h"

#include <QDBusConnection>
#include <QLoggingCategory>

using namespace Qt::StringLiterals;

WaydroidApplicationDBusClient::WaydroidApplicationDBusClient(const QString &packageName, QObject *parent)
    : QObject{parent}
    , m_packageName{packageName}
    , m_interface{new OrgKdePlasmashellWaydroidApplicationInterface{u"org.kde.plasmashell"_s,
                                                                    u"/Waydroid/Application/%1"_s.arg(packageName),
                                                                    QDBusConnection::sessionBus(),
                                                                    this}}
{
    // Check if the service is already running
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.plasmashell"_s)) {
        m_connected = true;
        if (m_interface->isValid()) {
            connectSignals();
            // Initialize properties
            updateName();
            updatePackageName();
        }
    }

    connect(QDBusConnection::sessionBus().interface(),
            &QDBusConnectionInterface::serviceOwnerChanged,
            this,
            [this](const QString &service, const QString &oldOwner, const QString &newOwner) {
                if (service == u"org.kde.plasmashell"_s) {
                    if (newOwner.isEmpty()) {
                        // Service stopped
                        m_connected = false;
                    } else if (oldOwner.isEmpty()) {
                        // Service started
                        m_connected = true;
                        if (m_interface->isValid()) {
                            connectSignals();
                            updateName();
                            updatePackageName();
                        }
                    }
                }
            });
}

QString WaydroidApplicationDBusClient::name() const
{
    return m_name;
}

QString WaydroidApplicationDBusClient::packageName() const
{
    return m_packageName;
}

bool WaydroidApplicationDBusClient::isValid() const
{
    return m_connected && m_interface && m_interface->isValid() && !m_name.isEmpty() && !m_packageName.isEmpty();
}

void WaydroidApplicationDBusClient::connectSignals()
{
    // For applications, properties don't typically change after creation
    // but we can still connect for consistency
}

void WaydroidApplicationDBusClient::updateName()
{
    if (!m_connected || !m_interface->isValid()) {
        return;
    }

    m_name = m_interface->name();
}

void WaydroidApplicationDBusClient::updatePackageName()
{
    if (!m_connected || !m_interface->isValid()) {
        return;
    }

    // Package name should match what we already have, but verify
    const QString dbusPackageName = m_interface->packageName();
    if (dbusPackageName != m_packageName) {
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Package name mismatch: expected" << m_packageName << "got" << dbusPackageName;
    }
}
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

WaydroidApplicationDBusClient::WaydroidApplicationDBusClient(const QString &objectPath, QObject *parent)
    : QObject{parent}
    , m_interface{new OrgKdePlasmashellWaydroidApplicationInterface{u"org.kde.plasmashell"_s, objectPath, QDBusConnection::sessionBus(), this}}
{
    // Check if the service is already running
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.plasmashell"_s)) {
        m_connected = true;
        if (m_interface->isValid()) {
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

void WaydroidApplicationDBusClient::updateName()
{
    auto reply = m_interface->name();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        const auto name = reply.argumentAt<0>();

        if (m_name != name) {
            m_name = name;
            Q_EMIT nameChanged();
        }
    });
}

void WaydroidApplicationDBusClient::updatePackageName()
{
    auto reply = m_interface->name();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        const auto packageName = reply.argumentAt<0>();

        if (m_packageName != packageName) {
            m_packageName = packageName;
            Q_EMIT packageNameChanged();
        }
    });
}
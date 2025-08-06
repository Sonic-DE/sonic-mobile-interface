/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroiddbusclient.h"

#include <QClipboard>
#include <QDBusServiceWatcher>
#include <QGuiApplication>

WaydroidDBusClient::WaydroidDBusClient(QObject *parent)
    : QObject{parent}
    , m_interface{new OrgKdePlasmashellInterface{QStringLiteral("org.kde.plasmashell"), QStringLiteral("/Waydroid"), QDBusConnection::sessionBus(), this}}
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
    connect(m_interface, &OrgKdePlasmashellInterface::statusChanged, this, &WaydroidDBusClient::updateStatus);
    connect(m_interface, &OrgKdePlasmashellInterface::sessionStatusChanged, this, &WaydroidDBusClient::updateSessionStatus);
    connect(m_interface, &OrgKdePlasmashellInterface::ipAddressChanged, this, &WaydroidDBusClient::updateIpAddress);
}

WaydroidDBusClient::Status WaydroidDBusClient::status() const
{
    return m_status;
}

WaydroidDBusClient::SessionStatus WaydroidDBusClient::sessionStatus() const
{
    return m_sessionStatus;
}

QString WaydroidDBusClient::ipAddress() const
{
    return m_ipAddress;
}

void WaydroidDBusClient::updateStatus()
{
    auto reply = m_interface->status();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        m_status = static_cast<Status>(reply.argumentAt<0>());
        Q_EMIT statusChanged();
    });
}

void WaydroidDBusClient::updateSessionStatus()
{
    auto reply = m_interface->status();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        m_sessionStatus = static_cast<SessionStatus>(reply.argumentAt<0>());
        Q_EMIT sessionStatusChanged();
    });
}

void WaydroidDBusClient::updateSystemType()
{
    auto reply = m_interface->systemType();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        m_systemType = static_cast<SystemType>(reply.argumentAt<0>());
        Q_EMIT sessionStatusChanged();
    });
}

void WaydroidDBusClient::updateIpAddress()
{
    auto reply = m_interface->ipAddress();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        m_ipAddress = reply.argumentAt<0>();
        Q_EMIT ipAddressChanged();
    });
}

void WaydroidDBusClient::updateAndroidId()
{
    auto reply = m_interface->androidId();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        m_androidId = reply.argumentAt<0>();
        Q_EMIT androidIdChanged();
    });
}

void WaydroidDBusClient::updateMultiWindows()
{
    auto reply = m_interface->multiWindows();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        m_multiWindows = reply.argumentAt<0>();
        Q_EMIT multiWindowsChanged();
    });
}

void WaydroidDBusClient::updateSuspend()
{
    auto reply = m_interface->suspend();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        m_suspend = reply.argumentAt<0>();
        Q_EMIT suspendChanged();
    });
}

void WaydroidDBusClient::updateUevent()
{
    auto reply = m_interface->uevent();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        m_uevent = reply.argumentAt<0>();
        Q_EMIT ueventChanged();
    });
}

void WaydroidDBusClient::copyToClipboard(const QString text)
{
    qGuiApp->clipboard()->setText(text);
}
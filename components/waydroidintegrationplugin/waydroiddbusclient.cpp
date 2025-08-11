/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroiddbusclient.h"

#include <QClipboard>
#include <QCoroDBusPendingReply>
#include <QGuiApplication>

using namespace Qt::StringLiterals;

WaydroidDBusClient::WaydroidDBusClient(QObject *parent)
    : QObject{parent}
    , m_interface{new OrgKdePlasmashellInterface{u"org.kde.plasmashell"_s, u"/Waydroid"_s, QDBusConnection::sessionBus(), this}}
{
    // Check if the service is already running
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(u"org.kde.plasmashell"_s)) {
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
                if (service == u"org.kde.plasmashell"_s) {
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
    connect(m_interface, &OrgKdePlasmashellInterface::downloadStatusChanged, this, [this](double downloaded, double total, double speed) {
        Q_EMIT downloadStatusChanged(downloaded, total, speed);
    });
    connect(m_interface, &OrgKdePlasmashellInterface::sessionStatusChanged, this, &WaydroidDBusClient::updateSessionStatus);
    connect(m_interface, &OrgKdePlasmashellInterface::systemTypeChanged, this, &WaydroidDBusClient::updateSystemType);
    connect(m_interface, &OrgKdePlasmashellInterface::ipAddressChanged, this, &WaydroidDBusClient::updateIpAddress);
    connect(m_interface, &OrgKdePlasmashellInterface::androidIdChanged, this, &WaydroidDBusClient::updateAndroidId);
    connect(m_interface, &OrgKdePlasmashellInterface::multiWindowsChanged, this, &WaydroidDBusClient::updateMultiWindows);
    connect(m_interface, &OrgKdePlasmashellInterface::suspendChanged, this, &WaydroidDBusClient::updateSuspend);
    connect(m_interface, &OrgKdePlasmashellInterface::ueventChanged, this, &WaydroidDBusClient::updateUevent);
    connect(m_interface, &OrgKdePlasmashellInterface::errorOccurred, this, [this](const QString title, const QString message) {
        Q_EMIT errorOccurred(title, message);
    });

    updateStatus();
    updateSessionStatus();
    updateSystemType();
    updateIpAddress();
    updateAndroidId();
    updateMultiWindows();
    updateSuspend();
    updateUevent();
}

WaydroidDBusClient::Status WaydroidDBusClient::status() const
{
    return m_status;
}

WaydroidDBusClient::SessionStatus WaydroidDBusClient::sessionStatus() const
{
    return m_sessionStatus;
}

WaydroidDBusClient::SystemType WaydroidDBusClient::systemType() const
{
    return m_systemType;
}

QString WaydroidDBusClient::ipAddress() const
{
    return m_ipAddress;
}

QString WaydroidDBusClient::androidId() const
{
    return m_androidId;
}

QCoro::Task<> WaydroidDBusClient::setMultiWindowsTask(const bool multiWindows)
{
    auto pendingReply = m_interface->setMultiWindows(multiWindows);
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::setMultiWindows(const bool multiWindows)
{
    return setMultiWindowsTask(multiWindows);
}

bool WaydroidDBusClient::multiWindows() const
{
    return m_multiWindows;
}

QCoro::Task<> WaydroidDBusClient::setSuspendTask(const bool suspend)
{
    auto pendingReply = m_interface->setSuspend(suspend);
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::setSuspend(const bool suspend)
{
    return setSuspendTask(suspend);
}

bool WaydroidDBusClient::suspend() const
{
    return m_suspend;
}

QCoro::Task<> WaydroidDBusClient::setUeventTask(const bool uevent)
{
    auto pendingReply = m_interface->setUevent(uevent);
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::setUevent(const bool multiWindows)
{
    return setUeventTask(multiWindows);
}

QCoro::Task<> WaydroidDBusClient::refreshSessionInfoTask()
{
    auto pendingReply = m_interface->refreshSessionInfo();
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::refreshSessionInfo()
{
    return refreshSessionInfoTask();
}

bool WaydroidDBusClient::uevent() const
{
    return m_uevent;
}

QCoro::Task<> WaydroidDBusClient::initializeTask(const SystemType systemType, const RomType romType, const bool forced)
{
    auto pendingReply = m_interface->initialize(systemType, romType, forced);
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::initialize(const SystemType systemType, const RomType romType, const bool forced)
{
    return initializeTask(systemType, romType, forced);
}

QCoro::Task<> WaydroidDBusClient::startSessionTask()
{
    auto pendingReply = m_interface->startSession();
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::startSession()
{
    return startSessionTask();
}

QCoro::Task<> WaydroidDBusClient::stopSessionTask()
{
    auto pendingReply = m_interface->stopSession();
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::stopSession()
{
    return stopSessionTask();
}

QCoro::Task<> WaydroidDBusClient::resetWaydroidTask()
{
    auto pendingReply = m_interface->resetWaydroid();
    co_await pendingReply;
}

QCoro::QmlTask WaydroidDBusClient::resetWaydroid()
{
    return resetWaydroidTask();
}

void WaydroidDBusClient::updateStatus()
{
    auto reply = m_interface->status();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        const auto status = static_cast<Status>(reply.argumentAt<0>());

        if (m_status != status) {
            m_status = status;
            Q_EMIT statusChanged();
        }
    });
}

void WaydroidDBusClient::updateSessionStatus()
{
    auto reply = m_interface->sessionStatus();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        const auto sessionStatus = static_cast<SessionStatus>(reply.argumentAt<0>());

        if (m_sessionStatus != sessionStatus) {
            m_sessionStatus = sessionStatus;
            Q_EMIT sessionStatusChanged();
        }
    });
}

void WaydroidDBusClient::updateSystemType()
{
    auto reply = m_interface->systemType();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<int> reply = *watcher;
        const auto systemType = static_cast<SystemType>(reply.argumentAt<0>());

        if (m_systemType != systemType) {
            m_systemType = systemType;
            Q_EMIT systemTypeChanged();
        }
    });
}

void WaydroidDBusClient::updateIpAddress()
{
    auto reply = m_interface->ipAddress();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        const auto ipAddress = reply.argumentAt<0>();

        if (m_ipAddress != ipAddress) {
            m_ipAddress = ipAddress;
            Q_EMIT ipAddressChanged();
        }
    });
}

void WaydroidDBusClient::updateAndroidId()
{
    auto reply = m_interface->androidId();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<QString> reply = *watcher;
        const auto androidId = reply.argumentAt<0>();

        if (m_androidId != androidId) {
            m_androidId = androidId;
            Q_EMIT androidIdChanged();
        }
    });
}

void WaydroidDBusClient::updateMultiWindows()
{
    auto reply = m_interface->multiWindows();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        const auto multiWindows = reply.argumentAt<0>();

        if (m_multiWindows != multiWindows) {
            m_multiWindows = multiWindows;
            Q_EMIT multiWindowsChanged();
        }
    });
}

void WaydroidDBusClient::updateSuspend()
{
    auto reply = m_interface->suspend();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        const auto suspend = reply.argumentAt<0>();

        if (m_suspend != suspend) {
            m_suspend = suspend;
            Q_EMIT suspendChanged();
        }
    });
}

void WaydroidDBusClient::updateUevent()
{
    auto reply = m_interface->uevent();
    auto watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](auto watcher) {
        QDBusPendingReply<bool> reply = *watcher;
        const auto uevent = reply.argumentAt<0>();

        if (m_uevent != uevent) {
            m_uevent = uevent;
            Q_EMIT ueventChanged();
        }
    });
}

void WaydroidDBusClient::copyToClipboard(const QString text)
{
    qGuiApp->clipboard()->setText(text);
}
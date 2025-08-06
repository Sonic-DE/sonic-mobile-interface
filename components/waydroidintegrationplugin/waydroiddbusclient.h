/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "plasmashellwaydroidinterface.h"
#include "waydroiddbusobject.h"

#include <QCoro/QCoroCore>
#include <QCoro/QCoroQmlTask>
#include <QDBusServiceWatcher>
#include <QObject>
#include <QString>

#include <qqmlregistration.h>

class WaydroidDBusClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(SessionStatus sessionStatus READ sessionStatus NOTIFY sessionStatusChanged)
    Q_PROPERTY(SystemType systemType READ systemType NOTIFY systemTypeChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString androidId READ androidId NOTIFY androidIdChanged)
    Q_PROPERTY(bool multiWindows READ multiWindows NOTIFY multiWindowsChanged)
    Q_PROPERTY(bool suspend READ suspend NOTIFY suspendChanged)
    Q_PROPERTY(bool uevent READ uevent NOTIFY ueventChanged)

public:
    explicit WaydroidDBusClient(QObject *parent = nullptr);

    /**
     * @enum Status
     * @brief Defines the possible installation statuses of the Waydroid service.
     */
    enum Status {
        NotSupported = WaydroidDBusObject::NotSupported,
        NotInitialized = WaydroidDBusObject::NotInitialized,
        Initializing = WaydroidDBusObject::Initializing,
        Initialized = WaydroidDBusObject::Initialized,
        Resetting = WaydroidDBusObject::Resetting,
    };
    Q_ENUM(Status)

    /**
     * @enum SessionStatus
     * @brief Defines the possible states of a Waydroid session.
     */
    enum SessionStatus {
        SessionStopped = WaydroidDBusObject::SessionStopped,
        SessionStarting = WaydroidDBusObject::SessionStarting,
        SessionRunning = WaydroidDBusObject::SessionRunning,
    };
    Q_ENUM(SessionStatus)

    /**
     * @enum SystemType
     * @brief Defines the types of Android systems supported by Waydroid.
     */
    enum SystemType {
        Vanilla = WaydroidDBusObject::Vanilla, ///< Vanilla Android system.
        Foss = WaydroidDBusObject::Foss, ///< Free and Open Source Software variant.
        Gapps = WaydroidDBusObject::Gapps, ///< Variant with Google Apps included.
        UnknownSystemType = WaydroidDBusObject::UnknownSystemType
    };
    Q_ENUM(SystemType)

    /**
     * @enum RomType
     * @brief Defines the types of ROMs supported by Waydroid.
     *
     * @todo Add OTA ROM with custom system url and vendor url
     */
    enum RomType {
        Lineage = WaydroidDBusObject::Lineage, ///< LineageOS ROM.
        Bliss = WaydroidDBusObject::Bliss ///< Bliss ROM.
    };
    Q_ENUM(RomType)

    [[nodiscard]] Status status() const;
    [[nodiscard]] SessionStatus sessionStatus() const;
    [[nodiscard]] SystemType systemType() const;
    [[nodiscard]] QString ipAddress() const;
    [[nodiscard]] QString androidId() const;
    [[nodiscard]] bool multiWindows() const;
    [[nodiscard]] bool suspend() const;
    [[nodiscard]] bool uevent() const;

    Q_INVOKABLE void copyToClipboard(const QString text);

Q_SIGNALS:
    void statusChanged();
    // download and total is in MB and speed in Kbps
    void downloadStatusChanged(double downloaded, double total, double speed);
    void sessionStatusChanged();
    void systemTypeChanged();
    void ipAddressChanged();
    void androidIdChanged();
    void multiWindowsChanged();
    void suspendChanged();
    void ueventChanged();

    void errorOccurred(const QString title, const QString message);

private Q_SLOTS:
    void updateStatus();
    void updateSessionStatus();
    void updateSystemType();
    void updateIpAddress();
    void updateAndroidId();
    void updateMultiWindows();
    void updateSuspend();
    void updateUevent();

private:
    void connectSignals();

    OrgKdePlasmashellInterface *m_interface;
    QDBusServiceWatcher *m_watcher;

    Status m_status{NotInitialized};
    SessionStatus m_sessionStatus{SessionStopped};
    SystemType m_systemType{UnknownSystemType};
    QString m_ipAddress{""};
    QString m_androidId{""};

    // Waydroid props. See https://docs.waydro.id/usage/waydroid-prop-options
    bool m_multiWindows{false};
    bool m_suspend{false};
    bool m_uevent{false};

    bool m_connected{false};
};
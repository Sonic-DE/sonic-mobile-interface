/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

#include <qqmlregistration.h>
#include <qtmetamacros.h>

class WaydroidState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(SessionStatus sessionStatus READ sessionStatus NOTIFY sessionStatusChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(bool multiWindows READ multiWindows WRITE setMultiWindows NOTIFY multiWindowsChanged)
    Q_PROPERTY(bool suspend READ suspend WRITE setSuspend NOTIFY suspendChanged)
    Q_PROPERTY(bool uevent READ uevent WRITE setUevent NOTIFY ueventChanged)

public:
    WaydroidState(QObject *parent = nullptr);

    enum Status {
        NotSupported = 0,
        NotInitialized,
        Initialiazing,
        Initialized,
        FailedToInitialize
    };
    Q_ENUM(Status)

    enum SessionStatus {
        SessionStopped = 0,
        SessionStarting,
        SessionRunning
    };
    Q_ENUM(SessionStatus)

    enum SystemType {
        Vanilla = 0,
        Foss,
        Gapps
    };
    Q_ENUM(SystemType)

    enum RomType {
        Lineage = 0,
        Bliss
    };
    Q_ENUM(RomType)

    Q_INVOKABLE void refreshSupportsInfo();
    Q_INVOKABLE void refreshSessionInfo();
    Q_INVOKABLE void refreshPropsInfo();
    Q_INVOKABLE void initialize(const SystemType systemType, const RomType romType, const bool forced = false);
    Q_INVOKABLE void startSession();
    Q_INVOKABLE void stopSession();

    Status status() const;
    SessionStatus sessionStatus() const;
    QString ipAddress() const;
    bool multiWindows() const;
    void setMultiWindows(const bool multiWindows);
    bool suspend() const;
    void setSuspend(const bool suspend);
    bool uevent() const;
    void setUevent(const bool uevent);

Q_SIGNALS:
    void statusChanged();
    void sessionStatusChanged();
    void ipAddressChanged();
    void multiWindowsChanged();
    void suspendChanged();
    void ueventChanged();

private:
    Status m_status{NotInitialized};
    SessionStatus m_sessionStatus{SessionStopped};
    QString m_ipAddress{""};

    // Waydroid props. See https://docs.waydro.id/usage/waydroid-prop-options
    bool m_multiWindows{false};
    bool m_suspend{false};
    bool m_uevent{false};

    QString fetchSessionInfo();
    QString fetchPropValue(const QString key, const QString defaultValue);
    bool writePropValue(const QString key, const QString value);
};

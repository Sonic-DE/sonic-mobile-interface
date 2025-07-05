/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "waydroidutil.h"
#include <QObject>

#include <qqmlregistration.h>
#include <qtmetamacros.h>

class WaydroidState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    WaydroidState(QObject *parent = nullptr);

    enum Status {
        NotSupported = 0,
        NotInitialized,
        Initialiazing,
        Initialized,
        FailedToInitialized
    };
    Q_ENUM(Status)

    void checkSupports();
    Status status() const;

Q_SIGNALS:
    void statusChanged();

private:
    WaydroidUtil m_waydroidUtil{nullptr};
    Status m_status{Status::NotInitialized};
};

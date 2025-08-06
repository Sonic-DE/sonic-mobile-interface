/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QDBusServiceWatcher>
#include <QObject>
#include <QString>
#include <qqmlregistration.h>

class WaydroidDBusClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit WaydroidDBusClient(QObject *parent = nullptr);

private:
    void connectSignals();

    OrgKdePlasmashellInterface *m_interface;
    QDBusServiceWatcher *m_watcher;

    bool m_connected = false;
}
/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "plasmashellwaydroidapplicationinterface.h"

#include <QDBusServiceWatcher>
#include <QObject>
#include <QString>

#include <qqmlregistration.h>

/**
 * This class provides a DBus client interface for individual Waydroid applications.
 * It connects to WaydroidApplicationDBusObject instances via DBus.
 *
 * @author Florian RICHER <florian.richer@protonmail.com>
 */
class WaydroidApplicationDBusClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString packageName READ packageName CONSTANT)

public:
    typedef std::shared_ptr<WaydroidApplicationDBusClient> Ptr;

    explicit WaydroidApplicationDBusClient(const QString &packageName, QObject *parent = nullptr);

    QString name() const;
    QString packageName() const;

    bool isValid() const;

private Q_SLOTS:
    void updateName();
    void updatePackageName();

private:
    QString m_packageName;
    QString m_name;
    OrgKdePlasmashellWaydroidApplicationInterface *m_interface;
    QDBusServiceWatcher *m_watcher;
    bool m_connected{false};

    void connectSignals();
};
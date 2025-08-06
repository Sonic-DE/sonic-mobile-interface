/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QString>

#include <qqmlregistration.h>

class WaydroidDBusObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasmashell")

public:
    WaydroidDBusObject(QObject *parent = nullptr);

    // called by QML
    Q_INVOKABLE void registerObject();

private:
    bool m_initialized{false};
};
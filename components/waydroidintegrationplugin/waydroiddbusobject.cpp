/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroiddbusobject.h"
#include "waydroidintegrationplugin_debug.h"

#include <QDBusConnection>
#include <QLoggingCategory>

using namespace Qt::StringLiterals;

WaydroidDBusObject::WaydroidDBusObject(QObject *parent)
    : QObject{parent}
{
}

void WaydroidDBusObject::registerObject()
{
    if (!m_initialized) {
        QDBusConnection::sessionBus().registerObject(u"/Waydroid"_s, this);
        m_initialized = true;
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Initialized";
    }
}
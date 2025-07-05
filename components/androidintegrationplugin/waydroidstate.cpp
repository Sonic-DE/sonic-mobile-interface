/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidstate.h"
#include "waydroidutil.h"

WaydroidState::WaydroidState(QObject *parent)
    : QObject{parent}
    , m_waydroidUtil{WaydroidUtil(this)}
{
    checkSupports();
}

void WaydroidState::checkSupports()
{
    if (!m_waydroidUtil.isAvailable()) {
        m_status = WaydroidState::Status::NotSupported;
        Q_EMIT statusChanged();
    }

    if (m_waydroidUtil.isInitialized()) {
        m_status = WaydroidState::Status::Initialized;
    } else {
        m_status = WaydroidState::Status::NotInitialized;
    }

    Q_EMIT statusChanged();
}

WaydroidState::Status WaydroidState::status() const
{
    return m_status;
}

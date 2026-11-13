/*
 * SPDX-FileCopyrightText: 2022 by Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "powermenuutil.h"

#include <sessionmanagement.h>

PowerMenuUtil::PowerMenuUtil(QObject *parent)
    : QObject{parent}
{
}

void PowerMenuUtil::openShutdownScreen()
{
    SessionManagement sessionManagement;
    // All options, as on TV; a shut-down-only prompt hides Log Out and Restart
    sessionManagement.requestLogoutPrompt();
}


// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "windowlistener.h"

WindowListener::WindowListener(QObject *parent)
    : QObject{parent}
{
}

WindowListener *WindowListener::instance()
{
    static WindowListener *listener = new WindowListener();
    return listener;
}

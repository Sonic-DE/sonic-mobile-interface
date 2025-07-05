/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <qobject.h>

class WaydroidUtil : public QObject
{
    Q_OBJECT

public:
    WaydroidUtil(QObject *parent = nullptr);

    bool isAvailable();
    bool isInitialized();
};

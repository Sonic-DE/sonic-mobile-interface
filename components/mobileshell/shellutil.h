/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class ShellUtil : public QObject
{
    Q_OBJECT

public:
    static ShellUtil *instance();
};

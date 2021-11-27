/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "shellutil.h"

ShellUtil *ShellUtil::instance()
{
    static ShellUtil *inst = new ShellUtil();
    return inst;
}

// SPDX-FileCopyrightText: 2023 by Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cellularutil.h"

#include <QDebug>
#include <QRegularExpression>

CellularUtil::CellularUtil(QObject *parent)
    : QObject{parent}
{
}

/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidutil.h"
#include <qdebug.h>
#include <qprocess.h>

#define WAYDROID_COMMAND "waydroid"

WaydroidUtil::WaydroidUtil(QObject *parent)
    : QObject{parent}
{
}

bool WaydroidUtil::isAvailable()
{
    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND);
    process->waitForFinished();
    return process->exitCode() == 0;
}

bool WaydroidUtil::isInitialized()
{
    QStringList arguments;
    arguments << "status";

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    const QString output = process->readAllStandardOutput();
    return !output.contains("WayDroid is not initialized");
}

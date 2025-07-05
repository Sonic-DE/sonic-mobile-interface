/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidstate.h"
#include <qprocess.h>

#define WAYDROID_COMMAND "waydroid"

WaydroidState::WaydroidState(QObject *parent)
    : QObject{parent}
{
    checkSupports();
}

void WaydroidState::checkSupports()
{
    int exitCode = QProcess::execute(WAYDROID_COMMAND);
    if (exitCode != 0) {
        m_status = WaydroidState::Status::NotSupported;
        Q_EMIT statusChanged();
    }

    QStringList arguments;
    arguments << "status";

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    const QString output = process->readAllStandardOutput();
    if (!output.contains("WayDroid is not initialized")) {
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

void WaydroidState::initialize(const SystemType systemType, const RomType romType, const bool forced)
{
    m_status = WaydroidState::Status::Initialiazing;
    Q_EMIT statusChanged();

    QStringList arguments;
    arguments << "init";

    arguments << "-s";
    switch (systemType) {
    case SystemType::Vanilla:
        arguments << "VANILLA";
        break;
    case SystemType::Foss:
        arguments << "FOSS";
        break;
    case SystemType::Gapps:
        arguments << "GAPPS";
        break;
    }

    arguments << "-r";
    switch (romType) {
    case RomType::Lineage:
        arguments << "lineage";
        break;
    case RomType::Bliss:
        arguments << "bliss";
        break;
    }

    if (forced) {
        arguments << "-f";
    }

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    if (process->exitCode() == 0) {
        m_status = WaydroidState::Status::Initialized;
    } else {
        m_status = WaydroidState::Status::FailedToInitialize;
    }

    Q_EMIT statusChanged();
}

/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidstate.h"
#include <QTimer>
#include <qdebug.h>
#include <qprocess.h>
#include <qregularexpression.h>

#include <KAuth/Action>
#include <KAuth/ExecuteJob>

using namespace Qt::StringLiterals;

#define WAYDROID_COMMAND "waydroid"

static const QRegularExpression sessionRegExp(u"Session:\\s*(\\w+)"_s);

WaydroidState::WaydroidState(QObject *parent)
    : QObject{parent}
{
    refreshSupportsInfo();
    refreshSessionInfo();
}

void WaydroidState::refreshSupportsInfo()
{
    const int exitCode = QProcess::execute(WAYDROID_COMMAND);
    if (exitCode != 0) {
        m_status = NotSupported;
        Q_EMIT statusChanged();
    }

    const QString output = fetchSessionInfo();
    if (!output.contains("WayDroid is not initialized")) {
        m_status = Initialized;
    } else {
        m_status = NotInitialized;
    }
    Q_EMIT statusChanged();
}

void WaydroidState::refreshSessionInfo()
{
    if (m_status != Initialized) {
        return;
    }

    const QString output = fetchSessionInfo();

    const QRegularExpressionMatch sessionMatch = sessionRegExp.match(output);
    if (sessionMatch.hasMatch() && sessionMatch.lastCapturedIndex() > 0) {
        const QString matchedString = sessionMatch.captured(sessionMatch.lastCapturedIndex());
        m_sessionStatus = matchedString.contains("RUNNING") ? SessionRunning : SessionStopped;
    } else {
        m_sessionStatus = SessionStopped;
    }
    Q_EMIT sessionStatusChanged();
}

void WaydroidState::initialize(const SystemType systemType, const RomType romType, const bool forced)
{
    if (m_status == Initialiazing) {
        return;
    }

    m_status = Initialiazing;
    Q_EMIT statusChanged();

    QString systemTypeArg;
    switch (systemType) {
    case SystemType::Vanilla:
        systemTypeArg = "VANILLA";
        break;
    case SystemType::Foss:
        systemTypeArg = "FOSS";
        break;
    case SystemType::Gapps:
        systemTypeArg = "GAPPS";
        break;
    }

    QString romTypeArg;
    switch (romType) {
    case RomType::Lineage:
        romTypeArg = "lineage";
        break;
    case RomType::Bliss:
        romTypeArg = "bliss";
        break;
    }

    QVariantMap args = {{u"systemType"_s, systemTypeArg}, {u"romType"_s, romTypeArg}, {u"forced"_s, forced}};

    KAuth::Action writeAction(u"org.kde.plasma.mobileshell.waydroidhelper.initialize"_s);
    writeAction.setHelperId(u"org.kde.plasma.mobileshell.waydroidhelper"_s);
    writeAction.setArguments(args);
    writeAction.setTimeout(3600000); // HACK: 1 hour to wait installation

    KAuth::ExecuteJob *job = writeAction.execute();
    if (job->exec()) {
        m_status = Initialized;
    } else {
        m_status = FailedToInitialize;
        qWarning() << "KAuth returned an error code:" << job->error() << " message: " << job->errorString();
    }

    Q_EMIT statusChanged();
}

void WaydroidState::startSession()
{
    if (m_sessionStatus == SessionStarting || m_sessionStatus == SessionRunning) {
        return;
    }

    m_sessionStatus = SessionStarting;
    Q_EMIT sessionStatusChanged();

    QStringList arguments;
    arguments << "session" << "start";

    // Don't wait for result because the command is blocking
    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);

    QTimer::singleShot(1000, [this]() {
        m_sessionStatus = SessionRunning;
        Q_EMIT sessionStatusChanged();
    });
}

void WaydroidState::stopSession()
{
    if (m_sessionStatus == SessionStopped) {
        return;
    }

    QStringList arguments;
    arguments << "session" << "stop";

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    if (process->exitCode() == 0) {
        m_sessionStatus = SessionStopped;
        Q_EMIT sessionStatusChanged();
    } else {
        qWarning() << "Failed to stop the Waydroid session: " << process->readAllStandardError();
    }
}

WaydroidState::Status WaydroidState::status() const
{
    return m_status;
}

WaydroidState::SessionStatus WaydroidState::sessionStatus() const
{
    return m_sessionStatus;
}

QString WaydroidState::fetchSessionInfo()
{
    QStringList arguments;
    arguments << "status";

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    return process->readAllStandardOutput();
}
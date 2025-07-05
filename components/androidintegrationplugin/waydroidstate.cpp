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
#define MULTI_WINDOWS_PROP_KEY "persist.waydroid.multi_windows"
#define SUSPEND_PROP_KEY "persist.waydroid.suspend"
#define UEVENT_PROP_KEY "persist.waydroid.uevent"

static const QRegularExpression sessionRegExp(u"Session:\\s*(\\w+)"_s);

WaydroidState::WaydroidState(QObject *parent)
    : QObject{parent}
{
    // Connect it-self to auto-refresh when required status has changed
    connect(this, &WaydroidState::statusChanged, this, &WaydroidState::refreshSessionInfo);
    connect(this, &WaydroidState::sessionStatusChanged, this, &WaydroidState::refreshPropsInfo);

    refreshSupportsInfo();
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

void WaydroidState::refreshPropsInfo()
{
    if (m_sessionStatus != SessionRunning) {
        return;
    }

    const QString multiWindowsPropValue = fetchPropValue(MULTI_WINDOWS_PROP_KEY, "false");
    m_multiWindows = multiWindowsPropValue == "true";
    Q_EMIT multiWindowsChanged();

    const QString suspendPropValue = fetchPropValue(SUSPEND_PROP_KEY, "true");
    m_suspend = suspendPropValue == "true";
    Q_EMIT suspendChanged();

    const QString ueventPropValue = fetchPropValue(UEVENT_PROP_KEY, "false");
    m_uevent = ueventPropValue == "true";
    Q_EMIT ueventChanged();
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

    // HACK: Let Waydroid session starting correctly
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

bool WaydroidState::multiWindows() const
{
    return m_multiWindows;
}

void WaydroidState::setMultiWindows(const bool multiWindows)
{
    if (m_multiWindows == multiWindows) {
        return;
    }

    const QString value = multiWindows ? "true" : "false";

    if (writePropValue(MULTI_WINDOWS_PROP_KEY, value)) {
        m_multiWindows = multiWindows;
        Q_EMIT multiWindowsChanged();
    }
}

bool WaydroidState::suspend() const
{
    return m_suspend;
}

void WaydroidState::setSuspend(const bool suspend)
{
    if (m_suspend == suspend) {
        return;
    }

    const QString value = suspend ? "true" : "false";

    if (writePropValue(SUSPEND_PROP_KEY, value)) {
        m_suspend = suspend;
        Q_EMIT suspendChanged();
    }
}

bool WaydroidState::uevent() const
{
    return m_uevent;
}

void WaydroidState::setUevent(const bool uevent)
{
    if (m_uevent == uevent) {
        return;
    }

    const QString value = uevent ? "true" : "false";

    if (writePropValue(UEVENT_PROP_KEY, value)) {
        m_uevent = uevent;
        Q_EMIT ueventChanged();
    }
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

QString WaydroidState::fetchPropValue(const QString key, const QString defaultValue)
{
    QStringList arguments;
    arguments << "prop" << "get" << key;

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    const QString commandOutput = process->readAllStandardOutput();
    const QString value = commandOutput.split("\n").first().trimmed();

    if (value.isEmpty()) {
        return defaultValue;
    }

    return value;
}

bool WaydroidState::writePropValue(const QString key, const QString value)
{
    QStringList arguments;
    arguments << "prop" << "set" << key << value;

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    return process->exitCode() == 0;
}
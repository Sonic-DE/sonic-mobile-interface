// SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
// SPDX-FileCopyrightText: 2022-2025 by Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "recordutil.h"

#include <QStandardPaths>
#include <QUrl>

#include <KLocalizedString>
#include <KNotification>

#include <Sonic/Logging/Logging>

SONIC_LOGGING_COMPONENT("org.sonicde.mobile.quicksetting.record")
SONIC_LOGGING_AUTO_INSTALL()

using namespace Qt::StringLiterals;

namespace {
QString helperPath()
{
    const QByteArray overridden = qgetenv("SONIC_MOBILE_SCREENRECORDER");
    return overridden.isEmpty() ? QStringLiteral(SCREENRECORDER_PATH) : QString::fromLocal8Bit(overridden);
}
}

RecordUtil::RecordUtil(QObject *parent)
    : QObject{parent}
{
    setState(State::Idle);
}

RecordUtil::~RecordUtil()
{
    // Let a running helper finish its file on its own.
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        m_process->waitForFinished(5000);
    }
}

bool RecordUtil::startRecording(const QString &screenName)
{
    if (m_state != State::Idle) {
        return false;
    }
    const QString base = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + u"/screen-recording"_s;
    QStringList arguments{u"--output"_s, base};
    if (!screenName.isEmpty()) {
        arguments << u"--screen"_s << screenName;
    }

    m_output.clear();
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::ForwardedErrorChannel);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RecordUtil::readOutput);
    connect(m_process, &QProcess::finished, this, &RecordUtil::processFinished);
    connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            sonicWarning() << "cannot run" << helperPath() << m_process->errorString();
            m_process->deleteLater();
            m_process = nullptr;
            setState(State::Idle);
        }
    });
    setState(State::Starting);
    m_process->start(helperPath(), arguments);
    return true;
}

void RecordUtil::stopRecording()
{
    if (!m_process || (m_state != State::Starting && m_state != State::Recording)) {
        return;
    }
    setState(State::Writing);
    m_process->terminate();
}

QString RecordUtil::quickSettingText() const
{
    return m_quickSettingText;
}

QString RecordUtil::quickSettingStatus() const
{
    return m_quickSettingStatus;
}

bool RecordUtil::isRecording() const
{
    return m_state != State::Idle;
}

void RecordUtil::showNotification(const QString &title, const QString &text, const QString &filePath)
{
    KNotification *notif = new KNotification(u"captured"_s);
    notif->setComponentName(u"plasma_mobile_quicksetting_record"_s);
    notif->setTitle(title);
    notif->setUrls({QUrl::fromLocalFile(filePath)});
    notif->setText(text);
    notif->sendEvent();
}

void RecordUtil::setState(State state)
{
    const bool wasRecording = isRecording();
    m_state = state;
    switch (state) {
    case State::Starting:
    case State::Recording:
        m_quickSettingText = i18nc("@info:status", "Recording…");
        m_quickSettingStatus = i18n("Screen is being captured…");
        break;
    case State::Writing:
        m_quickSettingText = i18nc("@info:status", "Writing…");
        m_quickSettingStatus = i18n("Please wait…");
        break;
    case State::Idle:
        m_quickSettingText = i18nc("@action:button", "Record Screen");
        m_quickSettingStatus = i18n("Tap to start recording");
        break;
    }
    Q_EMIT quickSettingTextChanged();
    Q_EMIT quickSettingStatusChanged();
    if (wasRecording != isRecording()) {
        Q_EMIT isRecordingChanged();
    }
}

void RecordUtil::readOutput()
{
    while (m_process->canReadLine()) {
        const QString line = QString::fromLocal8Bit(m_process->readLine()).trimmed();
        if (m_output.isEmpty() && !line.isEmpty()) {
            m_output = line;
            sonicInfo() << "recording the screen to" << m_output;
            if (m_state == State::Starting) {
                setState(State::Recording);
            }
        }
    }
}

void RecordUtil::processFinished(int exitCode, QProcess::ExitStatus status)
{
    m_process->deleteLater();
    m_process = nullptr;
    setState(State::Idle);
    if (status == QProcess::NormalExit && exitCode == 0 && !m_output.isEmpty()) {
        showNotification(i18n("New Screen Recording"), i18n("New Screen Recording saved in %1", m_output), m_output);
    } else if (status == QProcess::NormalExit && exitCode == 2) {
        m_quickSettingStatus = i18n("No encoders available for recording");
        Q_EMIT quickSettingStatusChanged();
    } else {
        sonicWarning() << "the screen recorder stopped with exit code" << exitCode << "status" << status;
    }
}

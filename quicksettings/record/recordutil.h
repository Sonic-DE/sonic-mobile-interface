// SPDX-FileCopyrightText: 2022 Aleix Pol Gonzalez <aleixpol@kde.org>
// SPDX-FileCopyrightText: 2022-2025 by Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QProcess>
#include <qqmlregistration.h>

// Runs the screen recorder helper, which grabs the X11 screen and encodes it outside the shell.
class RecordUtil : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString quickSettingText READ quickSettingText NOTIFY quickSettingTextChanged)
    Q_PROPERTY(QString quickSettingStatus READ quickSettingStatus NOTIFY quickSettingStatusChanged)
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY isRecordingChanged)

public:
    enum class State {
        Idle,
        Starting,
        Recording,
        Writing,
    };

    RecordUtil(QObject *parent = nullptr);
    ~RecordUtil() override;

    // Records the screen called screenName (QScreen::name(), the RandR output), or every screen when empty.
    Q_INVOKABLE bool startRecording(const QString &screenName);
    Q_INVOKABLE void stopRecording();

    QString quickSettingText() const;
    QString quickSettingStatus() const;
    bool isRecording() const;

    void showNotification(const QString &title, const QString &text, const QString &filePath);

Q_SIGNALS:
    void quickSettingTextChanged();
    void quickSettingStatusChanged();
    void isRecordingChanged();

private:
    void setState(State state);
    void readOutput();
    void processFinished(int exitCode, QProcess::ExitStatus status);

    State m_state = State::Idle;
    QString m_quickSettingText;
    QString m_quickSettingStatus;
    QString m_output;
    QProcess *m_process = nullptr;
};

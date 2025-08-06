/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroiddbusobject.h"
#include "waydroidintegrationplugin_debug.h"
#include "waydroidshared.h"

#include <QDBusConnection>
#include <QDir>
#include <QLoggingCategory>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTimer>

#include <KConfigGroup>
#include <KDesktopFile>
#include <KSandbox>

using namespace Qt::StringLiterals;

#define MULTI_WINDOWS_PROP_KEY "persist.waydroid.multi_windows"
#define SUSPEND_PROP_KEY "persist.waydroid.suspend"
#define UEVENT_PROP_KEY "persist.waydroid.uevent"

static const QRegularExpression sessionRegExp(u"Session:\\s*(\\w+)"_s);
static const QRegularExpression ipAdressRegExp(u"IP address:\\s*(\\d+\\.\\d+\\.\\d+\\.\\d+)"_s);
static const QRegularExpression systemOtaRegExp(u"system_ota\\s*=\\s*(\\S+)"_s);

WaydroidDBusObject::WaydroidDBusObject(QObject *parent)
    : QObject{parent}
{
}

void WaydroidDBusObject::registerObject()
{
    if (!m_dbusInitialized) {
        QDBusConnection::sessionBus().registerObject(u"/Waydroid"_s, this);
        m_dbusInitialized = true;

        // Connect it-self to auto-refresh when required status has changed
        connect(this, &WaydroidDBusObject::statusChanged, this, &WaydroidDBusObject::refreshSessionInfo);

        refreshSupportsInfo();
    }
}

WaydroidDBusObject::Status WaydroidDBusObject::status() const
{
    return m_status;
}

WaydroidDBusObject::SessionStatus WaydroidDBusObject::sessionStatus() const
{
    return m_sessionStatus;
}

void WaydroidDBusObject::refreshSupportsInfo()
{
    const QStringList arguments{u"-h"_s};

    auto process = QProcess(this);
    process.start(WAYDROID_COMMAND, arguments);
    process.waitForFinished();

    const int exitCode = process.exitCode();
    if (exitCode != 0) {
        m_status = NotSupported;
        Q_EMIT statusChanged();
        return;
    }

    const QString output = fetchSessionInfo();
    if (!output.contains("WayDroid is not initialized")) {
        m_status = Initialized;
    } else {
        m_status = NotInitialized;
    }
    Q_EMIT statusChanged();
}

void WaydroidDBusObject::refreshSessionInfo()
{
    if (m_status != Initialized) {
        return;
    }

    const QString output = fetchSessionInfo();

    const QString sessionMatchResult = extractRegExp(output, sessionRegExp);
    SessionStatus newSessionStatus;

    if (!sessionMatchResult.isEmpty()) {
        newSessionStatus = sessionMatchResult.contains("RUNNING") ? SessionRunning : SessionStopped;
    } else {
        newSessionStatus = SessionStopped;
    }

    if (m_sessionStatus != newSessionStatus) {
        m_sessionStatus = newSessionStatus;
        Q_EMIT sessionStatusChanged();
    }

    m_ipAddress = extractRegExp(output, ipAdressRegExp);
    Q_EMIT ipAddressChanged();
}

QString WaydroidDBusObject::fetchSessionInfo()
{
    const QStringList arguments{u"status"_s};

    auto process = QProcess(this);
    process.start(WAYDROID_COMMAND, arguments);
    process.waitForFinished();

    return process.readAllStandardOutput();
}

QString WaydroidDBusObject::fetchPropValue(const QString key, const QString defaultValue)
{
    const QStringList arguments{u"prop"_s, u"get"_s, key};

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

bool WaydroidDBusObject::writePropValue(const QString key, const QString value)
{
    const QStringList arguments{u"prop"_s, u"set"_s, key, value};

    auto process = QProcess(this);
    process.start(WAYDROID_COMMAND, arguments);
    process.waitForFinished();

    return process.exitCode() == 0;
}

QString WaydroidDBusObject::extractRegExp(const QString text, const QRegularExpression regExp) const
{
    const QRegularExpressionMatch match = regExp.match(text);

    if (match.hasMatch() && match.lastCapturedIndex() > 0) {
        return match.captured(match.lastCapturedIndex());
    } else {
        return "";
    }
}

void WaydroidDBusObject::checkSessionStarting(const int limit, const int tried)
{
    if (m_sessionStatus != SessionStarting) {
        return;
    }

    const QString output = fetchSessionInfo();
    const QString sessionMatchResult = extractRegExp(output, sessionRegExp);

    if (sessionMatchResult.contains("RUNNING")) {
        m_sessionStatus = SessionRunning;
        Q_EMIT sessionStatusChanged();
    } else if (tried == limit) {
        m_sessionStatus = SessionStopped;
        Q_EMIT sessionStatusChanged();
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to start the session after " << tried << " tries";
    } else {
        QTimer::singleShot(500, [this, tried, limit]() {
            checkSessionStarting(limit, tried + 1);
        });
    }
}

QString WaydroidDBusObject::desktopFileDirectory()
{
    auto dir = []() -> QString {
        if (KSandbox::isFlatpak()) {
            return qEnvironmentVariable("HOME") % u"/.local/share/applications/";
        }
        return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    }();

    QDir(dir).mkpath(QStringLiteral("."));

    return dir;
}

bool WaydroidDBusObject::removeWaydroidApplications()
{
    const QDir appsDir(desktopFileDirectory());
    const auto fileInfos = appsDir.entryInfoList(QDir::Files);
    if (fileInfos.length() < 1) {
        return false;
    }

    bool allFileRemoved = true;

    for (const auto &fileInfo : fileInfos) {
        if (fileInfo.fileName().contains(QStringView(u".desktop"))) {
            const KDesktopFile desktopFile(fileInfo.filePath());
            const KConfigGroup configGroup = desktopFile.desktopGroup();

            if (!configGroup.hasKey(u"Categories"_s)) {
                continue;
            }

            const auto categories = configGroup.readEntry(u"Categories"_s);
            if (!categories.contains(u"X-WayDroid-App"_s)) {
                continue;
            }

            QFile file(fileInfo.filePath());
            if (!file.remove()) {
                allFileRemoved &= false;
                qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to remove: " << desktopFile.name();
            }
        }
    }

    return allFileRemoved;
}
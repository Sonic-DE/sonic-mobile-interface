/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidapplicationlistmodel.h"
#include "waydroidapplicationdbusclient.h"
#include "waydroidintegrationplugin_debug.h"
#include "waydroidshared.h"

#include <QLoggingCategory>
#include <QProcess>
#include <QStringLiteral>

#include <KLocalizedString>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

WaydroidApplicationListModel::WaydroidApplicationListModel(WaydroidState *parent)
    : QAbstractListModel{parent}
    , m_waydroidState{parent}
    , m_refreshTimer{new QTimer{this}}
{
    // Waydroid does not return all installed applications immediately, so we need to refresh regularly.
    m_refreshTimer->setInterval(1s);
    m_refreshTimer->setSingleShot(false);
    m_refreshTimer->start();

    connect(m_refreshTimer, &QTimer::timeout, this, &WaydroidApplicationListModel::refreshApplications);
    connect(parent, &WaydroidState::sessionStatusChanged, this, &WaydroidApplicationListModel::refreshApplications);
}

WaydroidApplicationListModel::~WaydroidApplicationListModel() = default;

void WaydroidApplicationListModel::loadApplications(const QList<QString> packageNames)
{
    if (m_waydroidState->sessionStatus() != WaydroidState::SessionRunning) {
        return;
    }

    qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Reload waydroid apps";

    QMap<QString, int> appIdMap; // <packageName, index>
    for (int i = 0; i < m_applications.size(); ++i) {
        const auto &application = m_applications[i];
        appIdMap.insert(application->packageName(), i);
    }

    QList<QString> toInsert;

    for (const QString &packageName : packageNames) {
        auto it = appIdMap.find(packageName);
        if (it != appIdMap.end()) {
            // Application already in m_applications
            appIdMap.erase(it);
        } else {
            // Application needs to be inserted into m_applications
            toInsert.append(packageName);
        }
    }

    QList<int> toRemove;
    for (int index : appIdMap.values()) {
        toRemove.append(index);
    }

    std::sort(toRemove.begin(), toRemove.end());

    // Remove indices first, from end to start to avoid indicies changing
    for (int i = toRemove.size() - 1; i >= 0; --i) {
        int ind = toRemove[i];

        beginRemoveRows({}, ind, ind);
        m_applications.removeAt(ind);
        endRemoveRows();
    }

    // Append new elements
    for (const QString &packageName : toInsert) {
        beginInsertRows({}, m_applications.size(), m_applications.size());
        auto client = std::make_shared<WaydroidApplicationDBusClient>(packageName, this);
        m_applications.append(client);
        endInsertRows();
    }
}

void WaydroidApplicationListModel::refreshApplications()
{
    if (m_waydroidState->sessionStatus() != WaydroidState::SessionRunning) {
        return;
    }

    // Get the list of package names from the main Waydroid DBus service
    // This assumes the WaydroidDBusObject has already parsed and registered applications
    QDBusInterface waydroidInterface(u"org.kde.plasmashell"_s, u"/Waydroid"_s, u"org.kde.plasmashell"_s, QDBusConnection::sessionBus());

    if (!waydroidInterface.isValid()) {
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Cannot connect to Waydroid DBus interface";
        return;
    }

    // Instead of parsing ourselves, we look for registered application objects
    // For now, let's discover applications by trying to connect to known DBus paths
    // This is a simplified approach - in practice, you might want to add a method
    // to WaydroidDBusObject that returns the list of registered applications

    QStringList arguments = {u"app"_s, u"list"_s};
    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);

    connect(process, &QProcess::finished, this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode != 0 || exitStatus == QProcess::ExitStatus::CrashExit) {
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to run waydroid app list command: " << process->readAllStandardError();
            return;
        }

        const QByteArray data = process->readAllStandardOutput();
        if (data.isEmpty()) {
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Empty data: " << process->readAllStandardError();
            return;
        }

        qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Waydroid output: " << data;
        QTextStream output = QTextStream(data);

        QList<QString> packageNames;
        while (!output.atEnd()) {
            const QString line = output.readLine();
            if (line.startsWith("packageName:")) {
                const QString packageName = line.split(':').last().trimmed();
                if (!packageName.isEmpty()) {
                    packageNames.append(packageName);
                }
            }
        }

        loadApplications(packageNames);
    });
}

QHash<int, QByteArray> WaydroidApplicationListModel::roleNames() const
{
    return {{DelegateRole, QByteArrayLiteral("delegate")}, {NameRole, QByteArrayLiteral("name")}, {IdRole, QByteArrayLiteral("id")}};
}

QVariant WaydroidApplicationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_applications.count()) {
        return QVariant();
    }

    WaydroidApplicationDBusClient::Ptr app = m_applications.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case DelegateRole:
        return QVariant::fromValue(app.get());
    case NameRole:
        return app->name();
    case IdRole:
        return app->packageName();
    default:
        return QVariant();
    }
}

int WaydroidApplicationListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_applications.count();
}

void WaydroidApplicationListModel::installApk(const QString apkFile)
{
    const QStringList arguments{u"app"_s, u"install"_s, apkFile};

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);

    connect(process, &QProcess::finished, this, [this, apkFile, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            Q_EMIT actionFinished(i18n("Application has been installed"));
        } else {
            Q_EMIT errorOccurred(i18n("Installation Failed"));
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Error occured during installation of " << apkFile << ": " << process->readAllStandardError();
        }
    });
}

void WaydroidApplicationListModel::deleteApplication(const QString appId)
{
    const QStringList arguments{u"app"_s, u"remove"_s, appId};

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);

    connect(process, &QProcess::finished, this, [this, appId, process](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitCode);
        Q_UNUSED(exitStatus);

        const QByteArray errorLog = process->readAllStandardError();

        // "waydroid app remove" send log on stderr but keep exitCode to 0
        if (errorLog.isEmpty()) {
            Q_EMIT actionFinished(i18n("Application has been deleted"));
        } else {
            Q_EMIT errorOccurred(i18n("Application uninstall failed"));
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Error occured during uninstallation of " << appId << ": " << errorLog;
        }
    });
}
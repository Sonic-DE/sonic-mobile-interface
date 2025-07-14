/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waydroidintegrationplugin_debug.h"
#include <waydroidapplicationlistmodel.h>

#include <QProcess>
#include <QStringLiteral>

using namespace Qt::StringLiterals;

WaydroidApplicationListModel::WaydroidApplicationListModel(WaydroidState *parent)
    : QAbstractListModel{parent}
    , m_waydroidState{parent}
{
    connect(parent, &WaydroidState::sessionStatusChanged, this, &WaydroidApplicationListModel::refreshApplications);
}

WaydroidApplicationListModel::~WaydroidApplicationListModel() = default;

void WaydroidApplicationListModel::refreshApplications()
{
    if (m_waydroidState->sessionStatus() != WaydroidState::SessionRunning) {
        return;
    }

    qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Reload waydroid apps";

    QStringList arguments = {u"app"_s, u"list"_s};

    QProcess *process = new QProcess(m_waydroidState);
    process->start(u"waydroid"_s, arguments);
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to run waydroid app list command: " << process->readAllStandardError();
        return;
    }

    const QByteArray data = process->readAllStandardOutput();
    QTextStream output = QTextStream(data);

    beginResetModel();
    m_applications.clear();
    while (!output.atEnd()) {
        const WaydroidApplication::Ptr app = WaydroidApplication::fromWaydroidLog(parent(), output);
        if (app == nullptr) {
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to fetch the application: Maybe wrong QTextStream cursor position.";
            break;
        }

        qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Waydroid application found: " << app.get()->name() << " (" << app.get()->packageName() << ")";
        m_applications.append(app);
    }
    endResetModel();
}

QHash<int, QByteArray> WaydroidApplicationListModel::roleNames() const
{
    return {{DelegateRole, QByteArrayLiteral("delegate")}, {NameRole, QByteArrayLiteral("name")}};
}

QVariant WaydroidApplicationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_applications.count()) {
        return QVariant();
    }

    WaydroidApplication::Ptr app = m_applications.at(index.row());
    qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Role: " << role << "Query app" << app->name();

    switch (role) {
    case Qt::DisplayRole:
    case DelegateRole:
        return QVariant::fromValue(app.get());
    case NameRole:
        return app->name();
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

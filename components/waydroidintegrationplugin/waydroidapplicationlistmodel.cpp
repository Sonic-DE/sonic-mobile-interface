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
{
    connect(parent, &WaydroidState::sessionStatusChanged, this, [this, parent]() {
        if (parent->sessionStatus() == WaydroidState::SessionRunning) {
            refreshApplications();
        }
    });
}

WaydroidApplicationListModel::~WaydroidApplicationListModel() = default;

void WaydroidApplicationListModel::refreshApplications()
{
    m_applications.clear();
    qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Reload waydroid apps";

    QStringList arguments = {u"app"_s, u"list"_s};

    QProcess *process = new QProcess(this);
    process->start(u"waydroid"_s, arguments);
    process->waitForFinished();

    if (process->exitCode() != 0) {
        qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to run waydroid app list command: " << process->readAllStandardError();
        return;
    }

    QTextStream output = QTextStream(process->readAllStandardOutput());

    while (!output.atEnd()) {
        const WaydroidApplication::Ptr app = WaydroidApplication::fromWaydroidLog(parent(), output);
        if (app == nullptr) {
            qCWarning(WAYDROIDINTEGRATIONPLUGIN) << "Failed to fetch the application: Maybe wrong QTextStream cursor position.";
            break;
        }

        qCDebug(WAYDROIDINTEGRATIONPLUGIN) << "Waydroid application found: " << app.get()->name() << " (" << app.get()->packageName() << ")";
        m_applications.push_back(app);
    }
}

QHash<int, QByteArray> WaydroidApplicationListModel::roleNames() const
{
    return {{DelegateRole, QByteArrayLiteral("delegate")}};
}

QVariant WaydroidApplicationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    WaydroidApplication::Ptr app = m_applications.at(index.row());

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

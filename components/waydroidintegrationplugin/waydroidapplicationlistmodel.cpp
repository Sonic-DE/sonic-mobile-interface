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

WaydroidApplicationListModel::WaydroidApplicationListModel(QObject *parent)
    : QAbstractListModel{parent}
{
    load();
}

void WaydroidApplicationListModel::load()
{
    m_applications.clear();

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

        qCInfo(WAYDROIDINTEGRATIONPLUGIN) << "Waydroid application found: " << app.get()->name() << " (" << app.get()->packageName() << ")";
        m_applications.push_back(app);
    }
}
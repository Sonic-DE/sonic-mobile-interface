/*
    SPDX-FileCopyrightText: 2008 Alain Boyer <alainboyer@gmail.com>
    SPDX-FileCopyrightText: 2009 Matthieu Gallien <matthieu_gallien@yahoo.fr>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "statusnotifieritemjob.h"
#include <KWaylandExtras>
#include <KWindowSystem>

StatusNotifierItemJob::StatusNotifierItemJob(StatusNotifierItemSource *source, const QString &operation, QMap<QString, QVariant> &parameters, QObject *parent)
    : ServiceJob(source->objectName(), operation, parameters, parent)
    , m_source(source)
{
    // Queue connection, so that all 'deleteLater' are performed before we use updated menu.
    connect(source, SIGNAL(contextMenuReady(QMenu *)), this, SLOT(contextMenuReady(QMenu *)), Qt::QueuedConnection);
    connect(source, &StatusNotifierItemSource::activateResult, this, &StatusNotifierItemJob::activateCallback);
}

StatusNotifierItemJob::~StatusNotifierItemJob()
{
}

void StatusNotifierItemJob::start()
{
    if (operationName() == QLatin1String("Scroll")) {
        performJob();
        return;
    }

    QWindow *window = nullptr;
    const quint32 launchedSerial = KWaylandExtras::lastInputSerial(window);
    connect(KWaylandExtras::self(), &KWaylandExtras::xdgActivationTokenArrived, this, [this, launchedSerial](quint32 serial, const QString &token) {
        if (serial == launchedSerial) {
            m_source->provideXdgActivationToken(token);
            performJob();
        }
    });
    KWaylandExtras::requestXdgActivationToken(window, launchedSerial, {});
}

void StatusNotifierItemJob::performJob()
{
    if (operationName() == QLatin1String("Activate")) {
        m_source->activate(parameters()[QStringLiteral("x")].toInt(), parameters()[QStringLiteral("y")].toInt());
    } else if (operationName() == QLatin1String("SecondaryActivate")) {
        m_source->secondaryActivate(parameters()[QStringLiteral("x")].toInt(), parameters()[QStringLiteral("y")].toInt());
        setResult(0);
    } else if (operationName() == QLatin1String("ContextMenu")) {
        m_source->contextMenu(parameters()[QStringLiteral("x")].toInt(), parameters()[QStringLiteral("y")].toInt());
    } else if (operationName() == QLatin1String("Scroll")) {
        m_source->scroll(parameters()[QStringLiteral("delta")].toInt(), parameters()[QStringLiteral("direction")].toString());
        setResult(0);
    }
}

void StatusNotifierItemJob::activateCallback(bool success)
{
    if (operationName() == QLatin1String("Activate")) {
        setResult(QVariant(success));
    }
}

void StatusNotifierItemJob::contextMenuReady(QMenu *menu)
{
    if (operationName() == QLatin1String("ContextMenu")) {
        setResult(QVariant::fromValue((QObject *)menu));
    }
}

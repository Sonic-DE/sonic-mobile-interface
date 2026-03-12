// SPDX-FileCopyrightText: 2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "raiselockscreen.h"
#include "utils.h"

#include <QQuickItem>

#include <KWindowSystem>

RaiseLockscreen::RaiseLockscreen(QObject *parent)
    : QObject{parent}
{
}

RaiseLockscreen::~RaiseLockscreen()
{
}

QWindow *RaiseLockscreen::window() const
{
    return m_window;
}

void RaiseLockscreen::setWindow(QWindow *window)
{
    m_window = window;
    Q_EMIT windowChanged();
}

bool RaiseLockscreen::initialized() const
{
    return m_initialized;
}

void RaiseLockscreen::setInitialized(bool initialized)
{
    m_initialized = initialized;
    Q_EMIT initializedChanged();
}

void RaiseLockscreen::initializeOverlay(QQuickWindow *window)
{
    if (!window || window == m_window) {
        return;
    }

    setWindow(window);
    setOverlay();
}

void RaiseLockscreen::setOverlay()
{
}

bool RaiseLockscreen::eventFilter(QObject *watched, QEvent *event)
{
    auto window = qobject_cast<QQuickWindow *>(watched);
    if (window && event->type() == QEvent::PlatformSurface) {
        auto surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
        if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
            m_window->removeEventFilter(this);
            setOverlay();
        }
    }
    return false;
}

void RaiseLockscreen::raiseOverlay()
{
    if (!m_window) {
        qCWarning(LOGGING_CATEGORY) << "Unable to raise overlay: no window set";
        return;
    }

    if (!m_initialized) {
        qCWarning(LOGGING_CATEGORY) << "Unable to raise overlay: window is not initialized for lockscreen overlaying, trying anyway...";
    }
}

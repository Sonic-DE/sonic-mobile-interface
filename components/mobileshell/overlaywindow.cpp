// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "overlaywindow.h"

#include <KX11Extras>

OverlayWindow::OverlayWindow(QWindow *parent)
    : QQuickWindow(parent)
{
    setFlag(Qt::FramelessWindowHint, true);
    setFlag(Qt::WindowDoesNotAcceptFocus, true);
    setColor(Qt::transparent);

    KX11Extras::setOnAllDesktops(winId(), true);
    applyWindowProperties();

    // Qt derives the X11 window type and state from the flags again each time they change.
    connect(this, &QWindow::flagsChanged, this, &OverlayWindow::applyWindowProperties);
}

void OverlayWindow::applyWindowProperties()
{
    KX11Extras::setType(winId(), static_cast<NET::WindowType>(m_type));
    KX11Extras::setState(winId(), NET::KeepAbove | NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher);
}

OverlayWindow::Type OverlayWindow::type() const
{
    return m_type;
}

void OverlayWindow::setType(Type type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;
    applyWindowProperties();
    Q_EMIT typeChanged();
}

bool OverlayWindow::takeFocus() const
{
    return m_takeFocus;
}

void OverlayWindow::setTakeFocus(bool takeFocus)
{
    if (m_takeFocus == takeFocus) {
        return;
    }

    m_takeFocus = takeFocus;
    setFlag(Qt::WindowDoesNotAcceptFocus, !takeFocus);
    Q_EMIT takeFocusChanged();
}

#include "moc_overlaywindow.cpp"

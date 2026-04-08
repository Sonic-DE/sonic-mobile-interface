/*
 *  SPDX-FileCopyrightText: 2014 Antonis Tsiapaliokas <antonis.tsiapaliokas@kde.org>
 *  SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "windowutil.h"

#include <KApplicationTrader>

#include <QGuiApplication>

constexpr int ACTIVE_WINDOW_UPDATE_INVERVAL = 0;

WindowUtil::WindowUtil(QObject *parent)
    : QObject{parent}
    , m_activeWindowTimer{new QTimer{this}}
{
    // use 0 tick timer to update active window to ensure window state has finished changing
    m_activeWindowTimer->setSingleShot(true);
    m_activeWindowTimer->setInterval(ACTIVE_WINDOW_UPDATE_INVERVAL);
    connect(m_activeWindowTimer, &QTimer::timeout, this, &WindowUtil::updateActiveWindow);

    connect(this, &WindowUtil::activeWindowChanged, this, &WindowUtil::updateActiveWindowIsShell);
}

bool WindowUtil::isShowingDesktop() const
{
    return m_showingDesktop;
}

bool WindowUtil::activeWindowIsShell() const
{
    return m_activeWindowIsShell;
}

void WindowUtil::updateActiveWindow()
{
}

bool WindowUtil::hasCloseableActiveWindow() const
{
    return false;
}

bool WindowUtil::activateWindowByStorageId(const QString &storageId)
{
    Q_UNUSED(storageId);
    return false;
}

void WindowUtil::closeActiveWindow()
{
}

void WindowUtil::requestShowingDesktop(bool showingDesktop)
{
    Q_UNUSED(showingDesktop);
}

void WindowUtil::minimizeAll()
{
    qWarning() << "Ignoring request for minimizing all windows since window management hasn't been announced yet!";
}

void WindowUtil::unsetAllMinimizedGeometries(QQuickItem *parent)
{
    Q_UNUSED(parent);
}

void WindowUtil::updateShowingDesktop(bool showing)
{
    if (showing != m_showingDesktop) {
        m_showingDesktop = showing;
        Q_EMIT showingDesktopChanged(m_showingDesktop);
    }
}

void WindowUtil::updateActiveWindowIsShell()
{
}

void WindowUtil::forgetActiveWindow()
{
    Q_EMIT hasCloseableActiveWindowChanged();
}

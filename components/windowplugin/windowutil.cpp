/*
 *  SPDX-FileCopyrightText: 2014 Antonis Tsiapaliokas <antonis.tsiapaliokas@kde.org>
 *  SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "windowutil.h"

#include <KApplicationTrader>
#include <KWindowInfo>
#include <KWindowSystem>
#include <KX11Extras>
#include <NETWM>

#include <QCoreApplication>
#include <QGuiApplication>

constexpr int ACTIVE_WINDOW_UPDATE_INVERVAL = 0;

namespace {
// The window types an application window can have, as opposed to the shell's panels and overlays.
constexpr auto applicationWindowTypes = NET::NormalMask | NET::DialogMask | NET::UtilityMask;

xcb_connection_t *x11Connection()
{
    const auto x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    return x11App ? x11App->connection() : nullptr;
}

xcb_window_t x11RootWindow(xcb_connection_t *connection)
{
    return xcb_setup_roots_iterator(xcb_get_setup(connection)).data->root;
}

QByteArray withoutDesktopSuffix(QByteArray name)
{
    if (name.endsWith(".desktop")) {
        name.chop(8);
    }
    return name;
}
}

WindowUtil::WindowUtil(QObject *parent)
    : QObject{parent}
    , m_activeWindowTimer{new QTimer{this}}
    , m_showingDesktop{KWindowSystem::showingDesktop()}
{
    // use 0 tick timer to update active window to ensure window state has finished changing
    m_activeWindowTimer->setSingleShot(true);
    m_activeWindowTimer->setInterval(ACTIVE_WINDOW_UPDATE_INVERVAL);
    connect(m_activeWindowTimer, &QTimer::timeout, this, &WindowUtil::updateActiveWindow);

    connect(this, &WindowUtil::activeWindowChanged, this, &WindowUtil::updateActiveWindowIsShell);

    connect(KX11Extras::self(), &KX11Extras::activeWindowChanged, m_activeWindowTimer, qOverload<>(&QTimer::start));
    connect(KWindowSystem::self(), &KWindowSystem::showingDesktopChanged, this, &WindowUtil::updateShowingDesktop);

    updateActiveWindow();
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
    const WId window = KX11Extras::activeWindow();
    const KWindowInfo info(window, NET::WMWindowType | NET::WMPid, NET::WM2AllowedActions);

    const bool closeable = window && info.valid() && info.windowType(applicationWindowTypes) != NET::Unknown && info.actionSupported(NET::ActionClose);
    if (closeable != m_hasCloseableActiveWindow) {
        m_hasCloseableActiveWindow = closeable;
        Q_EMIT hasCloseableActiveWindowChanged();
    }

    m_activeWindow = window;
    Q_EMIT activeWindowChanged();
}

bool WindowUtil::hasCloseableActiveWindow() const
{
    return m_hasCloseableActiveWindow;
}

bool WindowUtil::activateWindowByStorageId(const QString &storageId)
{
    const QByteArray target = withoutDesktopSuffix(storageId.toUtf8());

    // Most recently stacked first, so the window the user last saw is the one activated.
    const auto windows = KX11Extras::stackingOrder();
    for (auto it = windows.crbegin(); it != windows.crend(); ++it) {
        const KWindowInfo info(*it, NET::WMWindowType, NET::WM2DesktopFileName | NET::WM2WindowClass);
        if (!info.valid() || info.windowType(applicationWindowTypes) == NET::Unknown) {
            continue;
        }

        // Applications without a desktop file name still commonly use it as their class.
        const QByteArray desktopFile = withoutDesktopSuffix(info.desktopFileName());
        if (desktopFile == target || (desktopFile.isEmpty() && info.windowClassClass().compare(target, Qt::CaseInsensitive) == 0)) {
            KX11Extras::forceActiveWindow(*it);
            return true;
        }
    }
    return false;
}

void WindowUtil::closeActiveWindow()
{
    auto *connection = x11Connection();
    if (connection && m_hasCloseableActiveWindow) {
        NETRootInfo(connection, NET::CloseWindow).closeWindowRequest(m_activeWindow);
    }
}

void WindowUtil::requestShowingDesktop(bool showingDesktop)
{
    KWindowSystem::setShowingDesktop(showingDesktop);
}

void WindowUtil::minimizeAll()
{
    const auto windows = KX11Extras::windows();
    for (const WId window : windows) {
        const KWindowInfo info(window, NET::WMWindowType | NET::XAWMState);
        if (info.valid() && info.windowType(applicationWindowTypes) != NET::Unknown && !info.isMinimized()) {
            KX11Extras::minimizeWindow(window);
        }
    }
}

void WindowUtil::unsetAllMinimizedGeometries(QQuickItem *parent)
{
    Q_UNUSED(parent);

    auto *connection = x11Connection();
    if (!connection) {
        return;
    }

    // X11 keeps one icon geometry per window, not one per window it minimizes into.
    const auto windows = KX11Extras::windows();
    for (const WId window : windows) {
        NETWinInfo info(connection, window, x11RootWindow(connection), NET::WMIconGeometry, NET::Properties2());
        info.setIconGeometry(NETRect{});
    }
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
    const KWindowInfo info(m_activeWindow, NET::WMPid);
    const bool isShell = m_activeWindow && info.valid() && info.pid() == QCoreApplication::applicationPid();
    if (isShell != m_activeWindowIsShell) {
        m_activeWindowIsShell = isShell;
        Q_EMIT activeWindowIsShellChanged();
    }
}

void WindowUtil::forgetActiveWindow()
{
    m_activeWindow = 0;
    m_hasCloseableActiveWindow = false;
    Q_EMIT hasCloseableActiveWindowChanged();
}

#include "moc_windowutil.cpp"

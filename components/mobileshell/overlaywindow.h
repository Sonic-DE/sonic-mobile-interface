// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QQuickWindow>
#include <qqmlregistration.h>

#include <netwm_def.h>

/**
 * Frameless window above normal windows, on all virtual desktops, for the shell's X11 overlays.
 */
class OverlayWindow : public QQuickWindow {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool takeFocus READ takeFocus WRITE setTakeFocus NOTIFY takeFocusChanged)

public:
    enum Type {
        Dock = NET::Dock,
        Notification = NET::Notification,
        OnScreenDisplay = NET::OnScreenDisplay,
    };
    Q_ENUM(Type)

    explicit OverlayWindow(QWindow *parent = nullptr);

    Type type() const;
    void setType(Type type);

    bool takeFocus() const;
    void setTakeFocus(bool takeFocus);

Q_SIGNALS:
    void typeChanged();
    void takeFocusChanged();

private:
    void applyWindowProperties();

    Type m_type = Dock;
    bool m_takeFocus = false;
};

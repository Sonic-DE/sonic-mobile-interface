/*
 *  SPDX-FileCopyrightText: 2021-2022 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings
import org.kde.plasma.private.mobileshell as MobileShell

/**
 * Component that triggers the opening and closing of an ActionDrawer when dragged on with touch or mouse.
 */
MobileShell.SwipeArea {
    id: root
    mode: MobileShell.SwipeArea.VerticalOnly

    // This may be given as null, if it needs to dynamically be loaded in
    required property ActionDrawer actionDrawer

    property bool swipeStarted: false

    function startSwipe() {
        if (!actionDrawer) {
            return;
        }
        if (actionDrawer.visible) {
            // ensure the action drawer state is consistent
            actionDrawer.closeImmediately();
        }
        actionDrawer.cancelAnimations();
        actionDrawer.dragging = true;
        actionDrawer.opened = false;

        // must be after properties other are set, we cannot have actionDrawer.updateState() be called
        actionDrawer.offset = 0;
        actionDrawer.oldOffset = 0;
        actionDrawer.visible = true;

        swipeStarted = true;
    }

    function endSwipe() {
        if (!actionDrawer) {
            return;
        }
        actionDrawer.dragging = false;
        actionDrawer.updateState();
        swipeStarted = false;
    }

    function updateOffset(offsetY) {
        if (!actionDrawer || !swipeStarted) {
            return;
        }
        actionDrawer.offset += offsetY;
    }

    anchors.fill: parent

    onSwipeStarted: (point) => {
        // if the user swiped from the top left, otherwise it's from the top right
        if (point.x < root.width / 2) {
            actionDrawer.openToPinnedMode = ShellSettings.Settings.actionDrawerTopLeftMode == ShellSettings.Settings.Pinned;
        } else {
            actionDrawer.openToPinnedMode = ShellSettings.Settings.actionDrawerTopRightMode == ShellSettings.Settings.Pinned;
        }

        startSwipe();
    }
    onSwipeEnded: endSwipe()
    onSwipeMove: (totalDeltaX, totalDeltaY, deltaX, deltaY) => updateOffset(deltaY);
}

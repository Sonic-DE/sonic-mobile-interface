/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

MouseArea {
    id: root
    
    required property ActionDrawer actionDrawer
    
    property int oldMouseY: 0

    function startSwipe(mouseX) {
        actionDrawer.cancelAnimations();
        actionDrawer.dragging = true;
        actionDrawer.offset = 0;
        actionDrawer.visible = true;
    }
    
    function endSwipe() {
        actionDrawer.dragging = false;
        actionDrawer.updateState();
    }
    
    function updateOffset(offsetY) {
        actionDrawer.offset += offsetY;
    }
    
    anchors.fill: parent
    onPressed: {
        oldMouseY = mouse.y;
        startSwipe(mouse.x);
    }
    onReleased: endSwipe()
    onCanceled: endSwipe()
    onPositionChanged: {
        updateOffset(mouse.y - oldMouseY);
        oldMouseY = mouse.y;
    }
}

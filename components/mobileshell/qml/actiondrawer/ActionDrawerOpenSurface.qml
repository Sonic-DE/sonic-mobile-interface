/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

MouseArea {
    id: root
    
    required property ActionDrawer actionDrawer
    
    property int oldMouseY: 0

    function startSwipe(mouseX) {
        slidingPanel.cancelAnimations();
        slidingPanel.drawerX = Math.min(Math.max(0, mouseX - slidingPanel.drawerWidth/2), slidingPanel.width - slidingPanel.contentItem.width)
        slidingPanel.userInteracting = true;
        slidingPanel.flickable.contentY = slidingPanel.closedContentY;
        slidingPanel.visible = true;
    }
    
    function endSwipe() {
        slidingPanel.userInteracting = false;
        slidingPanel.updateState();
    }
    
    function updateOffset(offsetY) {
        slidingPanel.updateOffset(offsetY);
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

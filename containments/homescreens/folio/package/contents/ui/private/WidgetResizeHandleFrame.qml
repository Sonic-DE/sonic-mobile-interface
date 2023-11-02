// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    // given by parent:

    property real widgetWidth
    property real widgetHeight
    property real widgetX
    property real widgetY

    property int widgetRow
    property int widgetColumn
    property int widgetGridWidth
    property int widgetGridHeight

    // filled here, given to parent:

    // what the drag intends for the dimensions and position of the widget
    property int widgetRowAfterDrag: 0
    property int widgetColumnAfterDrag: 0
    property int widgetGridWidthAfterDrag: 0
    property int widgetGridHeightAfterDrag: 0

    property var lockDrag: null

    // solely used here:

    property real startDragWidth: 0
    property real startDragHeight: 0
    property real startX: 0
    property real startY: 0

    property int startWidgetRow: 0
    property int startWidgetColumn: 0

    onWidgetWidthChanged: {
        if (lockDrag === null) updateDimensions();
    }
    onWidgetHeightChanged: {
        if (lockDrag === null) updateDimensions();
    }
    onWidgetXChanged: {
        if (lockDrag === null) updateDimensions();
    }
    onWidgetYChanged: {
        if (lockDrag === null) updateDimensions();
    }

    function updateDimensions() {
        handleContainer.width = widgetWidth;
        handleContainer.height = widgetHeight;
        handleContainer.x = widgetX;
        handleContainer.y = widgetY;
    }

    function startDrag() {
        startDragWidth = handleContainer.width;
        startDragHeight = handleContainer.height;
        startX = handleContainer.x;
        startY = handleContainer.y;

        startWidgetRow = root.widgetRow;
        startWidgetColumn = root.widgetColumn;

        widgetRowAfterDrag = startWidgetRow;
        widgetColumnAfterDrag = startWidgetColumn;
        widgetGridWidthAfterDrag = root.widgetGridWidth;
        widgetGridHeightAfterDrag = root.widgetGridHeight;
    }

    function snapEdges() {
        lockDrag = null;

        // snaps the bounds to what we ended up at
        widthAnim.to = widgetWidth;
        widthAnim.restart();
        heightAnim.to = widgetHeight;
        heightAnim.restart();
        xAnim.to = widgetX;
        xAnim.restart();
        yAnim.to = widgetY;
        yAnim.restart();
    }

    // updates the resized widget dimensions and position
    function updateAfterDrag() {
        const columnsMovedRight = Math.round((handleContainer.x - root.startX) / Folio.HomeScreenState.pageCellWidth);
        const rowsMovedDown = Math.round((handleContainer.y - root.startY) / Folio.HomeScreenState.pageCellHeight);

        widgetRowAfterDrag = startWidgetRow + rowsMovedDown;
        widgetColumnAfterDrag = startWidgetColumn + columnsMovedRight;
        widgetGridWidthAfterDrag = Math.round(handleContainer.width / Folio.HomeScreenState.pageCellWidth);
        widgetGridHeightAfterDrag = Math.round(handleContainer.height / Folio.HomeScreenState.pageCellHeight);
    }

    function pressedHandler(orientation) {
        if (root.lockDrag !== orientation) {
            root.startDrag();
            root.lockDrag = orientation;
        }
    }

    function dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) {
        if (root.lockDrag === orientation) {
            // update the handle container dimensions and position
            handleContainer.x = root.startX - leftEdgeDelta;
            handleContainer.y = root.startY - topEdgeDelta;
            handleContainer.width = root.startDragWidth + rightEdgeDelta + leftEdgeDelta;
            handleContainer.height = root.startDragHeight + bottomEdgeDelta + topEdgeDelta;

            // update the widget dimensions and position
            updateAfterDrag();
        }
    }

    function releaseHandler(orientation) {
        if (root.lockDrag === orientation) {
            root.snapEdges();
        }
    }

    Item {
        id: handleContainer

        NumberAnimation on width {
            id: widthAnim
            duration: 200
            easing.type: Easing.InOutQuad
        }

        NumberAnimation on height {
            id: heightAnim
            duration: 200
            easing.type: Easing.InOutQuad
        }

        NumberAnimation on x {
            id: xAnim
            duration: 200
            easing.type: Easing.InOutQuad
        }

        NumberAnimation on y {
            id: yAnim
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    WidgetResizeHandle {
        id: topLeftHandle
        orientation: WidgetHandlePosition.TopLeft

        x: handleContainer.x - (width / 2)
        y: handleContainer.y - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: topHandle
        orientation: WidgetHandlePosition.TopCenter

        x: handleContainer.x + (handleContainer.width / 2) - (width / 2)
        y: handleContainer.y - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: topRightHandle
        orientation: WidgetHandlePosition.TopRight

        x: handleContainer.x + handleContainer.width - (width / 2)
        y: handleContainer.y - (height / 2)

        onPressed: () => pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: leftHandle
        orientation: WidgetHandlePosition.LeftCenter

        x: handleContainer.x - (width / 2)
        y: handleContainer.y + (handleContainer.height / 2) - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: rightHandle
        orientation: WidgetHandlePosition.RightCenter

        x: handleContainer.x + handleContainer.width - (width / 2)
        y: handleContainer.y + (handleContainer.height / 2) - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: bottomLeftHandle
        orientation: WidgetHandlePosition.BottomLeft

        x: handleContainer.x - (width / 2)
        y: handleContainer.y + handleContainer.height - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: bottomHandle
        orientation: WidgetHandlePosition.BottomCenter

        x: handleContainer.x + (handleContainer.width / 2) - (width / 2)
        y: handleContainer.y + handleContainer.height - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }

    WidgetResizeHandle {
        id: bottomRightHandle
        orientation: WidgetHandlePosition.BottomRight

        x: handleContainer.x + handleContainer.width - (width / 2)
        y: handleContainer.y + handleContainer.height - (height / 2)

        onPressed: pressedHandler(orientation)
        onDragEvent: (leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta) => dragHandler(orientation, leftEdgeDelta, rightEdgeDelta, topEdgeDelta, bottomEdgeDelta)
        onReleased: releaseHandler(orientation)
    }
}

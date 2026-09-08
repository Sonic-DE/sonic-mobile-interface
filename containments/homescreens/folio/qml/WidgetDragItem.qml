// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg 1.0 as KSvg

import org.kde.plasma.components 3.0 as PC3
import plasma.applet.org.kde.plasma.mobile.homescreen.folio as Folio

import './delegate'
import './private'

pragma ComponentBehavior: Bound

// Placeholder item that the user sees as they drag widgets around.
// See DelegateDragItem for the equivalent for app delegates.
Item {
    id: root
    property Folio.HomeScreen folio

    width: widgetLoader.item ? widgetLoader.item.width : 0
    height: widgetLoader.item ? widgetLoader.item.height : 0

    property Folio.FolioWidget widget

    readonly property bool isWidgetDelegate: root.folio.HomeScreenState.dragState.dropDelegate
        && root.folio.HomeScreenState.dragState.dropDelegate.type === Folio.FolioDelegate.Widget
        && root.folio.HomeScreenState.dragState.dropDelegate.widget.visualApplet
    readonly property bool dropAnimationRunning: dragXAnim.running || dragYAnim.running

    visible: false
    x: Math.round(root.folio.HomeScreenState.delegateDragX)
    y: Math.round(root.folio.HomeScreenState.delegateDragY)

    function startDrag(widget) {
        root.widget = widget;
        visible = true;
    }

    function setXBinding() {
        x = Qt.binding(() => Math.round(root.folio.HomeScreenState.delegateDragX));
    }
    function setYBinding() {
        y = Qt.binding(() => Math.round(root.folio.HomeScreenState.delegateDragY));
    }

    // animate drop x
    XAnimator on x {
        id: dragXAnim
        running: false
        duration: Kirigami.Units.longDuration
        easing.type: Easing.OutCubic
        onFinished: {
            root.visible = false;
            root.widget = null;
            root.setXBinding();
        }
    }

    // animate drop y
    YAnimator on y {
        id: dragYAnim
        running: false
        duration: Kirigami.Units.longDuration
        easing.type: Easing.OutCubic
        onFinished: {
            root.visible = false;
            root.widget = null;
            root.setYBinding();
        }
    }

    Connections {
        id: stateWatcher
        target: root.folio.HomeScreenState

        function onDelegateDragStarted() {
            if (!root.isWidgetDelegate) {
                return;
            }

            root.startDrag(root.folio.HomeScreenState.dragState.dropDelegate.widget);
        }
    }

    Connections {
        target: root.folio.HomeScreenState.dragState

        // animate from when the delegate is dropped to its drop position
        function onDelegateDroppedAndPlaced() {
            if (!root.isWidgetDelegate) {
                return;
            }

            let dragState = root.folio.HomeScreenState.dragState;
            let dropPosition = dragState.candidateDropPosition;

            let pos = root.folio.HomeScreenState.getPageDelegateScreenPosition(dropPosition.page, dropPosition.pageRow, dropPosition.pageColumn);

            dragXAnim.to = pos.x;
            dragYAnim.to = pos.y;
            dragXAnim.restart();
            dragYAnim.restart();
        }

        // if the drop has been abandoned, just hide
        function onNewDelegateDropAbandoned() {
            root.visible = false;
        }
    }

    Loader {
        id: widgetLoader

        active: root.widget

        sourceComponent: WidgetDelegate {
            folio: root.folio
            widget: root.widget

            layer.enabled: true
            layer.effect: DarkenEffect {}
        }
    }
}

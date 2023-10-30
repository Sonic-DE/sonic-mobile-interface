// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import Qt5Compat.GraphicalEffects

import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg 1.0 as KSvg

import org.kde.plasma.components 3.0 as PC3
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

import '../private'

Item {
    id: root

    property int pageNum
    property int row
    property int column

    property real widgetWidth
    property real widgetHeight
    property real topWidgetBackgroundPadding
    property real leftWidgetBackgroundPadding

    property Folio.FolioWidget widget

    signal removeRequested()
    signal closed()

    function startOpen() {
        configOverlay.open();
    }

    function fullyOpen() {
        configPopup.open();
        configOverlay.close();
    }

    // HACK: this shows the config when we are in the "press to hold" state, prior to mouse release
    // we can't just open the popup, because the potential drag-and-drop swipe would get lost
    Item {
        id: configOverlay
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: configPopup.x
        anchors.topMargin: configPopup.y

        width: configPopup.width
        height: configPopup.height

        opacity: 0
        visible: opacity > 0

        NumberAnimation on opacity { id: configOverlayOpacityAnim; duration: 200 }

        function open() {
            configOverlayOpacityAnim.to = 1;
            configOverlayOpacityAnim.restart();
        }

        function animClose() {
            if (opacity !== 0) {
                configOverlayOpacityAnim.to = 0;
                configOverlayOpacityAnim.restart();
            }
        }

        function close() {
            opacity = 0;
        }

        Connections {
            target: Folio.HomeScreenState

            // if we are starting drag-and-drop, close the menu immediately
            function onSwipeStateChanged() {
                if (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate) {
                    configOverlay.animClose();
                    root.closed();
                }
            }
        }

        // the config overlay
        FastBlur {
            anchors.fill: parent
            source: configPopup.contentItem
            radius: 0
        }
    }

    QQC2.Popup {
        id: configPopup

        // offset so we can place the config menu appropriately
        x: (configBar.orientation === Orientation.Left) ? -(barWidth + barSpacing) : 0
        y: (configBar.orientation === Orientation.Above) ? -(barWidth + barSpacing) : 0

        width: root.width + ((configBar.orientation === Orientation.Above || configBar.orientation === Orientation.Below) ? 0 : (barWidth + barSpacing))
        height: root.height + ((configBar.orientation === Orientation.Above || configBar.orientation === Orientation.Below) ? (barWidth + barSpacing) : 0)

        onClosed: root.closed()

        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        closePolicy: QQC2.Popup.CloseOnEscape | QQC2.Popup.CloseOnPressOutsideParent

        readonly property real barWidth: Kirigami.Units.gridUnit * 3.5
        readonly property real barSpacing: Kirigami.Units.largeSpacing
        readonly property real minimumBarLength: Kirigami.Units.gridUnit * 8

        background: Item {}
        QQC2.Overlay.modal: Item {}

        exit: Transition {
            NumberAnimation { property: "opacity"; duration: 200; from: 1.0; to: 0.0 }
        }

        contentItem: MouseArea {
            id: configItem

            onClicked: configPopup.close()

            Item {
                id: widgetHandles
                width: root.widgetWidth
                height: root.widgetHeight

                anchors.top: parent.top
                anchors.left: parent.left

                anchors.topMargin: ((configBar.orientation === Orientation.Above) ? (configBar.height + configPopup.barSpacing) : 0) + root.topWidgetBackgroundPadding
                anchors.leftMargin: ((configBar.orientation === Orientation.Left) ? (configBar.width + configPopup.barSpacing) : 0) + root.leftWidgetBackgroundPadding

                WidgetResizeHandle {
                    id: topLeftHandle
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: -width
                    anchors.topMargin: -height
                }

                WidgetResizeHandle {
                    id: topHandle
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: -height
                }

                WidgetResizeHandle {
                    id: topRightHandle
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: -width
                    anchors.topMargin: -height
                }

                WidgetResizeHandle {
                    id: leftHandle
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: -width
                }

                WidgetResizeHandle {
                    id: rightHandle
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: -width
                }

                WidgetResizeHandle {
                    id: bottomLeftHandle
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: -width
                    anchors.bottomMargin: -height
                }

                WidgetResizeHandle {
                    id: bottomHandle
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: -height
                }

                WidgetResizeHandle {
                    id: bottomRightHandle
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: -width
                    anchors.bottomMargin: -height
                }
            }

            Item {
                id: configBar

                property int orientation: {
                    if (root.height > root.width) {
                        if (root.column === 0) {
                            return Orientation.Right;
                        } else {
                            return Orientation.Left;
                        }
                    } else {
                        if (root.row === 0) {
                            return Orientation.Below;
                        } else {
                            return Orientation.Above;
                        }
                    }
                }

                states: [
                    State {
                        name: "above"
                        when: configBar.orientation === Orientation.Above
                        AnchorChanges {
                            target: configBar
                            anchors.bottom: widgetHandles.top
                            anchors.horizontalCenter: widgetHandles.horizontalCenter
                        }
                        PropertyChanges {
                            configBar.anchors.bottomMargin: configPopup.barSpacing
                            configBar.width: Math.max(configPopup.minimumBarLength, root.width)
                            configBar.height: configPopup.barWidth
                        }
                    }, State {
                        name: "below"
                        when: configBar.orientation === Orientation.Below
                        AnchorChanges {
                            target: configBar
                            anchors.top: widgetHandles.bottom
                            anchors.horizontalCenter: widgetHandles.horizontalCenter
                        }
                        PropertyChanges {
                            configBar.anchors.topMargin: configPopup.barSpacing
                            configBar.width: Math.max(configPopup.minimumBarLength, root.width)
                            configBar.height: configPopup.barWidth
                        }
                    }, State {
                        name: "left"
                        when: configBar.orientation === Orientation.Left
                        AnchorChanges {
                            target: configBar
                            anchors.right: widgetHandles.left
                            anchors.verticalCenter: widgetHandles.verticalCenter
                        }
                        PropertyChanges {
                            configBar.anchors.rightMargin: configPopup.barSpacing
                            configBar.width: configPopup.barWidth
                            configBar.height: Math.max(configPopup.minimumBarLength, root.height)
                        }
                    }, State {
                        name: "right"
                        when: configBar.orientation === Orientation.Right
                        AnchorChanges {
                            target: configBar
                            anchors.left: widgetHandles.right
                            anchors.verticalCenter: widgetHandles.verticalCenter
                        }
                        PropertyChanges {
                            configBar.anchors.leftMargin: configPopup.barSpacing
                            configBar.width: configPopup.barWidth
                            configBar.height: Math.max(configPopup.minimumBarLength, root.height)
                        }
                    }
                ]

                KSvg.FrameSvgItem {
                    id: configBarBackground
                    anchors.fill: parent
                    enabledBorders: KSvg.FrameSvgItem.AllBorders
                    imagePath: 'widgets/background'
                }

                Flow {
                    spacing: Kirigami.Units.smallSpacing

                    anchors.fill: parent
                    anchors.leftMargin: configBarBackground.margins.left
                    anchors.rightMargin: configBarBackground.margins.right
                    anchors.topMargin: configBarBackground.margins.top
                    anchors.bottomMargin: configBarBackground.margins.bottom

                    Repeater {
                        model: root.widget.applet ? [...root.widget.applet.contextualActions, configureAppletAction, removeDelegateAction] : [removeDelegateAction]

                        delegate: PC3.Button {
                            display: PC3.ToolButton.IconOnly
                            width: (configBar.orientation === Orientation.Above || configBar.orientation === Orientation.Below)
                                        ? height : configPopup.barWidth - configBarBackground.margins.top - configBarBackground.margins.bottom
                            height: (configBar.orientation === Orientation.Left || configBar.orientation === Orientation.Right)
                                        ? width : configPopup.barWidth - configBarBackground.margins.left - configBarBackground.margins.right

                            text: modelData.text
                            icon.name: modelData.icon.name
                            onClicked: modelData.triggered()
                        }
                    }
                }
            }
        }
    }

    Kirigami.Action {
        id: removeDelegateAction
        icon.name: 'edit-delete-remove'
        text: i18n('Remove')
        onTriggered: root.removeRequested()
    }

    Kirigami.Action {
        id: configureAppletAction
        icon.name: 'settings-configure'
        text: i18n('Configure')
        onTriggered: root.widget.applet.internalAction('configure').trigger();
    }
}

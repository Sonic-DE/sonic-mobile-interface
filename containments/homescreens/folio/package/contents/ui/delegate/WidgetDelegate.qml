// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.ksvg 1.0 as KSvg

import org.kde.plasma.components 3.0 as PC3
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

import '../private'

Folio.WidgetContainer {
    id: root

    property Folio.FolioWidget widget
    property int row
    property int column

    implicitWidth: widget.gridWidth * Folio.HomeScreenState.pageCellWidth
    implicitHeight: widget.gridHeight * Folio.HomeScreenState.pageCellHeight
    width: implicitWidth
    height: implicitHeight

    signal removeRequested()

    onEditModeChanged: {
        if (editMode) {
            configPopup.open();
        }
    }

    function updateVisualApplet() {
        if (!widget.visualApplet) {
            return;
        }

        // widget.applet.userBackgroundHints = PlasmaCore.Types.NoBackground;

        //FIXME: make a way to instantiate fullRepresentationItem without the open/close dance
        widget.visualApplet.expanded = true;
        widget.visualApplet.expanded = false;

        widget.visualApplet.parent = widgetHolder;
        widget.visualApplet.anchors.fill = widgetHolder;
        widget.visualApplet.fullRepresentationItem.parent = widgetHolder;
        widget.visualApplet.fullRepresentationItem.anchors.fill = widgetHolder;
    }

    Component.onCompleted: {
        updateVisualApplet();
    }

    Connections {
        target: widget

        function onVisualAppletChanged() {
            if (!widget.visualApplet) {
                return;
            }

            root.updateVisualApplet();
        }
    }

    Item {
        id: widgetComponent
        anchors.fill: parent

        KSvg.FrameSvgItem {
            id: widgetBackground
            anchors.fill: parent
            enabledBorders: KSvg.FrameSvgItem.AllBorders
            imagePath: {
                if (!root.widget.applet || root.widget.applet.effectiveBackgroundHints === PlasmaCore.Types.NoBackground) {
                    return '';
                } else if (root.widget.applet.effectiveBackgroundHints & PlasmaCore.Types.StandardBackground) {
                    return 'widgets/background';
                } else if (root.widget.applet.effectiveBackgroundHints & PlasmaCore.Types.TranslucentBackground) {
                    return 'widgets/translucentbackground';
                }
                return '';
            }
        }

        Rectangle {
            id: temporaryBackground
            anchors.fill: parent
            visible: !root.widget.applet
            color: Qt.rgba(255, 255, 255, 0.3)
            radius: Kirigami.Units.smallSpacing
        }

        Item {
            id: widgetHolder
            anchors.fill: parent
            anchors.leftMargin: widgetBackground.margins.left
            anchors.rightMargin: widgetBackground.margins.right
            anchors.topMargin: widgetBackground.margins.top
            anchors.bottomMargin: widgetBackground.margins.bottom
        }

        // TODO implement blur behind, see plasma-workspace BasicAppletContainer for how to do this
        layer.enabled: root.widget.applet && root.widget.applet.effectiveBackgroundHints === PlasmaCore.Types.ShadowBackground
        layer.effect: DelegateShadow {}

        PC3.Label {
            id: noWidget
            visible: !root.widget.visualApplet
            color: 'white'
            wrapMode: Text.Wrap
            text: i18n('This widget was not found.')
            horizontalAlignment: Text.AlignHCenter

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }

        PC3.BusyIndicator {
            id: loadingIndicator
            anchors.centerIn: parent
            visible: root.widget.applet && root.widget.applet.busy
            running: visible
        }

        PC3.Button {
            id: configurationRequiredButton
            anchors.centerIn: parent
            text: i18n('Configure…')
            icon.name: 'configure'
            visible: root.widget.applet && root.widget.applet.configurationRequired
            onClicked: root.widget.applet.internalAction('configure').trigger();
        }
    }

    // PC3.Menu {
    //     id: menu
    //     title: "Context Menu"
    //     // closePolicy: PC3.Menu.CloseOnReleaseOutside | PC3.Menu.CloseOnEscape
    //
    //     Repeater {
    //         model: root.widget.applet ? [...root.widget.applet.contextualActions, configureAppletAction, removeDelegateAction] : []
    //         delegate: PC3.MenuItem {
    //             icon.name: modelData.iconName
    //             text: modelData.text
    //             onClicked: modelData.triggered()
    //         }
    //     }
    // }

    QQC2.Popup {
        id: configPopup
        width: root.width + ((configBar.orientation === Orientation.Above || configBar.orientation === Orientation.Below) ? 0 : (configBar.width + barSpacing))
        height: root.height + ((configBar.orientation === Orientation.Above || configBar.orientation === Orientation.Below) ? (configBar.height + barSpacing) : 0)

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

        // HACK: can't use enter transition because it seems to cause the popup to instantly close?
        onOpened: {
            openAnim.restart();
        }
        NumberAnimation on opacity { id: openAnim; from: 0; to: 1; duration: 200 }
        exit: Transition {
            NumberAnimation { property: "opacity"; duration: 200; from: 1.0; to: 0.0 }
        }

        contentItem: MouseArea {
            id: configItem

            onClicked: configPopup.close()

            Item {
                id: widgetHandles
                width: widgetHolder.width
                height: widgetHolder.height

                anchors.top: parent.top
                anchors.left: parent.left

                anchors.topMargin: ((configBar.orientation === Orientation.Above) ? (configBar.height + configPopup.barSpacing) : 0) + widgetBackground.margins.top
                anchors.leftMargin: ((configBar.orientation === Orientation.Left) ? (configBar.width + configPopup.barSpacing) : 0) + widgetBackground.margins.left

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

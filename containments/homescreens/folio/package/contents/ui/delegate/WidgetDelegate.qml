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

Item {
    id: root

    property Folio.FolioWidget widget

    implicitWidth: widget.gridWidth * Folio.HomeScreenState.pageCellWidth
    implicitHeight: widget.gridHeight * Folio.HomeScreenState.pageCellHeight
    width: implicitWidth
    height: implicitHeight

    function updateVisualApplet() {
        if (!widget.visualApplet) {
            return;
        }

        widget.userBackgroundHints = PlasmaCore.Types.NoBackground;

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

            console.log('visual applet changed');

            root.updateVisualApplet();
        }
    }

    // TODO temporary background
    Item {
        id: widgetComponent
        anchors.fill: parent

        KSvg.FrameSvgItem {
            id: background
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
            anchors.leftMargin: background.margins.left
            anchors.rightMargin: background.margins.right
            anchors.topMargin: background.margins.top
            anchors.bottomMargin: background.margins.bottom
        }

        // TODO implement blur behind, see plasma-workspace BasicAppletContainer for how to do this
        layer.enabled: root.widget.effectiveBackgroundHints === PlasmaCore.Types.ShadowBackground
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
            visible: root.widget.applet.busy
            running: visible
        }

        PC3.Button {
            id: configurationRequiredButton
            anchors.centerIn: parent
            text: i18n('Configure…')
            icon.name: 'configure'
            visible: root.widget.applet.configurationRequired
            onClicked: root.widget.applet.internalAction('configure').trigger();
        }
    }
}

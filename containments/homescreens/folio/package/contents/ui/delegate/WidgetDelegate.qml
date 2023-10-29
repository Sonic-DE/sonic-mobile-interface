// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    property Folio.FolioWidget widget

    implicitWidth: widget.gridWidth * Folio.HomeScreenState.pageCellWidth
    implicitHeight: widget.gridHeight * Folio.HomeScreenState.pageCellHeight
    width: implicitWidth
    height: implicitHeight

    // TODO temporary background
    Rectangle {
        id: background
        color: Qt.rgba(255, 255, 255, 0.3)
        radius: Kirigami.Units.smallSpacing
        anchors.fill: parent
    }

    function updateVisualApplet() {
        if (!widget.visualApplet) {
            return;
        }

        //FIXME: make a way to instantiate fullRepresentationItem without the open/close dance
        widget.visualApplet.expanded = true;
        widget.visualApplet.expanded = false;

        widget.visualApplet.parent = root;
        widget.visualApplet.anchors.fill = root;
        widget.visualApplet.fullRepresentationItem.parent = root;
        widget.visualApplet.fullRepresentationItem.anchors.fill = root;
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
}

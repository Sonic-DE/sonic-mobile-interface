// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Effects

import org.kde.kirigami 2.20 as Kirigami

import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings

AbstractDelegate {
    id: delegate

    property Folio.FolioApplicationFolder folder

    property alias folderItem: rect

    onAfterClickAnimation: {
        // launchApp();
    }

    contentItem: Rectangle {
        id: rect
        radius: Kirigami.Units.largeSpacing
        color: Qt.rgba(255, 255, 255, 0.3)

        Grid {
            id: previewGrid
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing * 2
            columns: 2
            spacing: Kirigami.Units.smallSpacing

            property var previews: delegate.folder.appPreviews

            Repeater {
                model: previewGrid.previews
                delegate: Kirigami.Icon {
                    implicitWidth: Math.round((previewGrid.width - previewGrid.spacing) / 2)
                    implicitHeight: Math.round((previewGrid.width - previewGrid.spacing) / 2)
                    source: modelData.icon
                }
            }
        }
    }
}



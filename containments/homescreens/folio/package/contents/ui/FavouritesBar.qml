// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.kirigami 2.10 as Kirigami

Item {
    id: root
    height: rowLayout.height

    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real effectiveContentWidth: width - leftMargin - rightMargin
    readonly property real horizontalMargin: Math.round(width * 0.05)

    readonly property real bottomMargin: 0
    readonly property real leftMargin: horizontalMargin
    readonly property real rightMargin: horizontalMargin

    readonly property real cellWidth: effectiveContentWidth / Math.min(Math.floor(effectiveContentWidth / (Kirigami.Units.iconSizes.large + Kirigami.Units.gridUnit * 2)), 8)
    readonly property real cellHeight: cellWidth + reservedSpaceForLabel

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    RowLayout {
        id: rowLayout
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.bottomMargin
        anchors.leftMargin: root.leftMargin
        anchors.rightMargin: root.rightMargin
        anchors.horizontalCenter: parent.horizontalCenter

        Repeater {
            model: Folio.FavouritesModel

            delegate: AppDrawerDelegate {
                id: delegate

                name: Folio.FolioSettings.showFavouritesAppLabels ? model.delegate.application.name : ""
                icon: model.delegate.application.icon
                storageId: model.delegate.application.storageId
                applicationRunning: model.delegate.application.running

                shadow: true

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: root.cellWidth
                Layout.preferredHeight: root.cellHeight

                width: root.cellWidth
                height: root.cellHeight
                reservedSpaceForLabel: root.reservedSpaceForLabel

                onDragStarted: (imageSource, x, y, mimeData) => {
                    // root.Drag.imageSource = imageSource;
                    // root.Drag.hotSpot.x = x;
                    // root.Drag.hotSpot.y = y;
                    // root.Drag.mimeData = { "text/x-plasma-phone-homescreen-launcher": mimeData };
                    //
                    // root.homeScreenState.closeAppDrawer()
                    //
                    // root.dragStarted()
                    // root.Drag.active = true;
                }
                onLaunch: (x, y, icon, title, storageId) => {
                    if (icon !== "") {
                        MobileShellState.ShellDBusClient.openAppLaunchAnimation(
                                icon,
                                title,
                                delegate.iconItem.Kirigami.ScenePosition.x + delegate.iconItem.width/2,
                                delegate.iconItem.Kirigami.ScenePosition.y + delegate.iconItem.height/2,
                                Math.min(delegate.iconItem.width, delegate.iconItem.height));
                    }

                    model.delegate.application.setMinimizedDelegate(index, delegate);
                    MobileShell.AppLaunch.launchOrActivateApp(storageId);
                }
            }
        }
    }
}

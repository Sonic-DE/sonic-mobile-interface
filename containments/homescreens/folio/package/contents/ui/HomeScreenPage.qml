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

    property var pageModel

    readonly property real horizontalMargin: Math.round(width * 0.05)
    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real effectiveContentWidth: width - horizontalMargin * 2

    readonly property real cellWidth: effectiveContentWidth / Folio.FolioSettings.homeScreenColumns
    readonly property real cellHeight: cellWidth + reservedSpaceForLabel

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    Repeater {
        model: root.pageModel

        delegate: Item {
            id: delegate

            property Folio.FolioPageDelegate pageDelegate: model.delegate
            property real row: pageDelegate.row
            property real column: pageDelegate.column

            implicitWidth: root.cellWidth
            implicitHeight: root.cellHeight
            width: root.cellWidth
            height: root.cellHeight

            x: column * root.cellWidth + root.horizontalMargin
            y: row * root.cellHeight + Kirigami.Units.gridUnit

            Loader {
                anchors.fill: parent

                sourceComponent: {
                    if (delegate.pageDelegate.type === Folio.FolioDelegate.Application) {
                        return appComponent;
                    } else if (delegate.pageDelegate.type === Folio.FolioDelegate.Application.Folder) {
                        return folderComponent;
                    } else {
                        return noneComponent;
                    }
                }
            }

            Component {
                id: noneComponent

                Item {}
            }

            Component {
                id: appComponent

                AppDrawerDelegate {
                    id: appDelegate

                    name: delegate.pageDelegate.application.name
                    icon: delegate.pageDelegate.application.icon
                    storageId: delegate.pageDelegate.application.storageId
                    applicationRunning: delegate.pageDelegate.application.running

                    shadow: true

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
                                    appDelegate.iconItem.Kirigami.ScenePosition.x + appDelegate.iconItem.width/2,
                                    appDelegate.iconItem.Kirigami.ScenePosition.y + appDelegate.iconItem.height/2,
                                    Math.min(appDelegate.iconItem.width, appDelegate.iconItem.height));
                        }

                        delegate.pageDelegate.application.setMinimizedDelegate(appDelegate);
                        MobileShell.AppLaunch.launchOrActivateApp(storageId);
                    }
                }
            }

            Component {
                id: folderComponent

                Item {}
            }
        }
    }
}

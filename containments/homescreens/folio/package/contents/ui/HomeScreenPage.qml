// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts

import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.kirigami 2.10 as Kirigami

Item {
    id: root

    property int pageNum

    property var pageModel
    property var homeScreenState
    property var homeScreen

    property int reservedSpaceForLabel
    property real cellWidth
    property real cellHeight

    // rectangle that shows when hovering over a spot to drop a delegate on
    Rectangle {
        id: dragDropFeedback
        color: Qt.rgba(255, 255, 255, 0.2)
        radius: Kirigami.Units.largeSpacing
        width: root.cellWidth
        height: root.cellHeight

        property var dropPosition: root.homeScreenState.dragState.candidateDropPosition

        visible: root.homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                    dropPosition.location === Folio.DelegateDragPosition.Pages &&
                    dropPosition.page === root.pageNum

        x: dropPosition.pageColumn * root.cellWidth
        y: dropPosition.pageRow * root.cellHeight
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

            x: column * root.cellWidth
            y: row * root.cellHeight

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

                    // don't show when in drag and drop mode
                    property var startPosition: root.homeScreenState.dragState.startPosition
                    opacity: (root.homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                                startPosition.location === Folio.DelegateDragPosition.Pages &&
                                startPosition.page === root.pageNum &&
                                startPosition.pageRow === delegate.pageDelegate.row &&
                                startPosition.pageColumn === delegate.pageDelegate.column) ? 0 : 1

                    // don't show label in drag and drop mode
                    labelOpacity: opacity

                    onPressAndHold: {
                        let mappedCoords = root.homeScreen.prepareStartDelegateDrag(appDelegate.delegateItem);
                        root.homeScreenState.startDelegatePageDrag(
                            mappedCoords.x,
                            mappedCoords.y,
                            root.pageNum,
                            delegate.pageDelegate.row,
                            delegate.pageDelegate.column
                        );

                        contextMenu.open();
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

                    onRightMousePress: {
                        contextMenu.open();
                    }

                    ContextMenuLoader {
                        id: contextMenu

                        // close menu when drag starts
                        Connections {
                            target: root.homeScreenState

                            function onSwipeStateChanged() {
                                if (root.homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate) {
                                    contextMenu.close();
                                }
                            }
                        }

                        actions: [
                            Kirigami.Action {
                                icon.name: "emblem-favorite"
                                text: i18n("Remove")
                                onTriggered: root.pageModel.removeDelegate(delegate.row, delegate.column)
                            }
                        ]
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

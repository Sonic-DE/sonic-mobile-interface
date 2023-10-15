// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.kirigami 2.10 as Kirigami

Item {
    id: root
    height: container.height

    property var homeScreen

    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real effectiveContentWidth: width - leftMargin - rightMargin
    readonly property real horizontalMargin: Math.round(width * 0.05)

    readonly property real bottomMargin: 0
    readonly property real leftMargin: horizontalMargin
    readonly property real rightMargin: horizontalMargin

    readonly property real cellWidth: effectiveContentWidth / Math.min(Math.floor(effectiveContentWidth / (Kirigami.Units.iconSizes.large + Kirigami.Units.gridUnit * 2)), 8)
    readonly property real cellHeight: cellWidth + reservedSpaceForLabel

    signal delegateDragRequested(var item)

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    Item {
        id: container
        height: cellHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.bottomMargin
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: Folio.FavouritesModel

            delegate: Item {
                id: delegate

                property var delegateModel: model.delegate
                property int index: model.index

                property var dragState: Folio.HomeScreenState.dragState
                property bool isAppHoveredOver: Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                                                dragState.dropDelegate &&
                                                dragState.dropDelegate.type === Folio.FolioDelegate.Application &&
                                                dragState.candidateDropPosition.location === Folio.DelegateDragPosition.Favourites &&
                                                dragState.candidateDropPosition.favouritesPosition === delegate.index

                x: model.xPosition
                anchors.verticalCenter: parent.verticalCenter

                Behavior on x {
                    NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
                }

                implicitWidth: root.cellWidth
                implicitHeight: root.cellHeight
                width: root.cellWidth
                height: root.cellHeight

                Loader {
                    anchors.fill: parent

                    sourceComponent: {
                        if (delegate.delegateModel.type === Folio.FolioDelegate.Application) {
                            return appComponent;
                        } else if (delegate.delegateModel.type === Folio.FolioDelegate.Folder) {
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

                    AppDelegate {
                        id: appDelegate
                        application: delegate.delegateModel.application
                        name: Folio.FolioSettings.showFavouritesAppLabels ? delegate.delegateModel.application.name : ""
                        shadow: true
                        reservedSpaceForLabel: root.reservedSpaceForLabel

                        turnToFolder: delegate.isAppHoveredOver
                        turnToFolderAnimEnabled: Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate

                        // don't show label in drag and drop mode
                        labelOpacity: delegate.opacity

                        onPressAndHold: {
                            let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.delegateModel, appDelegate.delegateItem);
                            Folio.HomeScreenState.startDelegateFavouritesDrag(
                                mappedCoords.x,
                                mappedCoords.y,
                                delegate.index
                            );

                            contextMenu.open();
                        }

                        onPressAndHoldReleased: {
                            // cancel the event if the delegate is not dragged
                            if (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.AwaitingDraggingDelegate) {
                                homeScreen.cancelDelegateDrag();
                            }
                        }

                        onRightMousePress: {
                            contextMenu.open();
                        }

                        ContextMenuLoader {
                            id: contextMenu

                            // close menu when drag starts
                            Connections {
                                target: Folio.HomeScreenState

                                function onSwipeStateChanged() {
                                    if (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate) {
                                        contextMenu.close();
                                    }
                                }
                            }

                            actions: [
                                Kirigami.Action {
                                    icon.name: "emblem-favorite"
                                    text: i18n("Remove")
                                    onTriggered: Folio.FavouritesModel.removeEntry(delegate.index)
                                }
                            ]
                        }
                    }
                }

                Component {
                    id: folderComponent

                    AppFolderDelegate {
                        id: appFolderDelegate
                        shadow: true
                        folder: delegate.delegateModel.folder
                        name: Folio.FolioSettings.showFavouritesAppLabels ? delegate.delegateModel.folder.name : ""
                        reservedSpaceForLabel: root.reservedSpaceForLabel

                        appHoveredOver: delegate.isAppHoveredOver

                        // don't show label in drag and drop mode
                        labelOpacity: delegate.opacity

                        onPressAndHold: {
                            let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.delegateModel, appFolderDelegate.delegateItem);
                            Folio.HomeScreenState.startDelegateFavouritesDrag(
                                mappedCoords.x,
                                mappedCoords.y,
                                delegate.index
                            );

                            contextMenu.open();
                        }

                        onPressAndHoldReleased: {
                            // cancel the event if the delegate is not dragged
                            if (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.AwaitingDraggingDelegate) {
                                root.homeScreen.cancelDelegateDrag();
                            }
                        }

                        onRightMousePress: {
                            contextMenu.open();
                        }

                        // TODO don't use loader, and move outside to a page to make it more performant
                        ContextMenuLoader {
                            id: contextMenu

                            // close menu when drag starts
                            Connections {
                                target: Folio.HomeScreenState

                                function onSwipeStateChanged() {
                                    if (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate) {
                                        contextMenu.close();
                                    }
                                }
                            }

                            actions: [
                                Kirigami.Action {
                                    icon.name: "emblem-favorite"
                                    text: i18n("Remove")
                                    onTriggered: Folio.FavouritesModel.removeEntry(delegate.index)
                                }
                            ]
                        }
                    }
                }
            }
        }
    }
}

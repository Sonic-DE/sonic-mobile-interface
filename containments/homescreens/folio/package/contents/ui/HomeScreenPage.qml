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

        property var dropPosition: Folio.HomeScreenState.dragState.candidateDropPosition
        property var startPosition: Folio.HomeScreenState.dragState.startPosition

        property bool dropIsStartPosition: startPosition.location === Folio.DelegateDragPosition.Pages &&
                                            startPosition.location === dropPosition.location &&
                                            startPosition.pageRow === dropPosition.pageRow &&
                                            startPosition.pageColumn === dropPosition.pageColumn

        visible: Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                    dropPosition.location === Folio.DelegateDragPosition.Pages &&
                    dropPosition.page === root.pageNum // &&
                    // TODO !dropIsStartPosition

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

            // don't show when in drag and drop mode
            property var startPosition: Folio.HomeScreenState.dragState.startPosition
            opacity: (Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                        startPosition.location === Folio.DelegateDragPosition.Pages &&
                        startPosition.page === root.pageNum &&
                        startPosition.pageRow === delegate.pageDelegate.row &&
                        startPosition.pageColumn === delegate.pageDelegate.column) ? 0 : 1

            Loader {
                anchors.fill: parent

                sourceComponent: {
                    if (delegate.pageDelegate.type === Folio.FolioDelegate.Application) {
                        return appComponent;
                    } else if (delegate.pageDelegate.type === Folio.FolioDelegate.Folder) {
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
                    application: delegate.pageDelegate.application
                    reservedSpaceForLabel: root.reservedSpaceForLabel

                    // don't show label in drag and drop mode
                    labelOpacity: delegate.opacity

                    onPressAndHold: {
                        let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.pageDelegate, appDelegate.delegateItem);
                        Folio.HomeScreenState.startDelegatePageDrag(
                            mappedCoords.x,
                            mappedCoords.y,
                            root.pageNum,
                            delegate.pageDelegate.row,
                            delegate.pageDelegate.column
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
                                onTriggered: root.pageModel.removeDelegate(delegate.row, delegate.column)
                            }
                        ]
                    }
                }
            }

            Component {
                id: folderComponent

                AppFolderDelegate {
                    id: appFolderDelegate
                    folder: delegate.pageDelegate.folder
                    reservedSpaceForLabel: root.reservedSpaceForLabel

                    // don't show label in drag and drop mode
                    labelOpacity: delegate.opacity

                    onPressAndHold: {
                        let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.pageDelegate, appFolderDelegate.delegateItem);
                        Folio.HomeScreenState.startDelegatePageDrag(
                            mappedCoords.x,
                            mappedCoords.y,
                            root.pageNum,
                            delegate.pageDelegate.row,
                            delegate.pageDelegate.column
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
                                onTriggered: root.pageModel.removeDelegate(delegate.row, delegate.column)
                            }
                        ]
                    }
                }
            }
        }
    }
}

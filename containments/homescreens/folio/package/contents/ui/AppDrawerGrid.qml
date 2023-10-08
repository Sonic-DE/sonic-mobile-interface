// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PC3
import org.kde.kirigami as Kirigami

import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

MobileShell.GridView {
    id: root
    cacheBuffer: cellHeight * 20
    reuseItems: true
    layer.enabled: true

    property var homeScreenState
    property var homeScreen

    // /*
    // * HACK: When the number of apps is less than the one that would fit in the first shown part of the drawer, make
    // * this flickable interactive, in order to steal inputs that would normally be delivered to home.
    // */
    // interactive: contentHeight <= height ? true : root.homeScreenState.appDrawerInteractive

    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real effectiveContentWidth: width - leftMargin - rightMargin
    readonly property real horizontalMargin: Math.round(width * 0.05)

    leftMargin: horizontalMargin
    rightMargin: horizontalMargin

    cellWidth: effectiveContentWidth / Math.min(Math.floor(effectiveContentWidth / (Kirigami.Units.iconSizes.large + Kirigami.Units.gridUnit * 2)), 8)
    cellHeight: cellWidth + reservedSpaceForLabel

    readonly property int columns: Math.floor(effectiveContentWidth / cellWidth)
    readonly property int rows: Math.ceil(root.count / columns)

    model: Folio.ApplicationListModel

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    delegate: AppDrawerDelegate {
        id: delegate
        name: model.applicationName
        applicationRunning: model.applicationRunning
        storageId: model.applicationStorageId
        icon: model.applicationIcon

        width: root.cellWidth
        height: root.cellHeight
        reservedSpaceForLabel: root.reservedSpaceForLabel

        onPressAndHold: {
            homeScreenState.closeAppDrawer();
            let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.delegateItem);
            root.homeScreenState.startDelegateAppDrawerDrag(
                mappedCoords.x,
                mappedCoords.y,
                model.applicationStorageId
            );
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

            Folio.ApplicationListModel.setMinimizedDelegate(index, delegate);
            MobileShell.AppLaunch.launchOrActivateApp(storageId);
        }
    }

    PC3.ScrollBar.vertical: PC3.ScrollBar {
        id: scrollBar
        interactive: true
        enabled: true
        implicitWidth: Kirigami.Units.smallSpacing

        Behavior on opacity {
            OpacityAnimator {
                duration: Kirigami.Units.longDuration * 2
                easing.type: Easing.InOutQuad
            }
        }

        contentItem: Rectangle {
            radius: width / 2
            color: Qt.rgba(1, 1, 1, 0.3)
        }
    }
}

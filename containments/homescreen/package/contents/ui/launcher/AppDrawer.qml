/*
 *  Copyright 2021 Marco Martin <mart@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3 as Controls

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PC3
//import org.kde.kquickcontrolsaddons 2.0
import org.kde.kirigami 2.10 as Kirigami

import org.kde.plasma.private.nanoshell 2.0 as NanoShell

import org.kde.phone.homescreen 1.0

GridView {
    id: view

    readonly property int columns: Math.floor(view.width / cellWidth)
    cellWidth: view.width / Math.floor(view.width / ((availableCellHeight - reservedSpaceForLabel) + units.smallSpacing*4))
    cellHeight: availableCellHeight
    clip: true

    signal launched
    signal dragStarted

    readonly property int reservedSpaceForLabel: metrics.height
    property int availableCellHeight: units.iconSizes.huge + reservedSpaceForLabel

    Controls.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: theme.defaultFont.pointSize * 0.9
    }

    model: ApplicationListModel {
        Component.onCompleted: loadApplications()
    }

    delegate: DrawerDelegate {
        id: delegate
        width: view.cellWidth
        height: view.cellHeight
        reservedSpaceForLabel: view.reservedSpaceForLabel

        onDragStarted: view.dragStarted()
        onLaunch: (x, y, icon, title) => {
            if (icon !== "") {
                NanoShell.StartupFeedback.open(
                        icon,
                        title,
                        delegate.iconItem.Kirigami.ScenePosition.x + delegate.iconItem.width/2,
                        delegate.iconItem.Kirigami.ScenePosition.y + delegate.iconItem.height/2,
                        Math.min(delegate.iconItem.width, delegate.iconItem.height));
            }
            view.launched();
        }
    }
}

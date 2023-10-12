// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Effects

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    property var source

    MultiEffect {
        id: effect
        anchors.fill: parent

        // HACK: prevents crashes if the soruce is invalid
        source: Folio.HomeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate ? root.source : emptyItem
    }

    Item { id: emptyItem }

    layer.enabled: true
    layer.effect: DelegateShadow {}
}

// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Effects

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root
    width: Folio.HomeScreenState.pageCellWidth
    height: Folio.HomeScreenState.pageCellHeight

    property Folio.FolioDelegate delegate

    DelegateIconLoader {
        id: loader
        anchors.centerIn: parent

        delegate: root.delegate

        layer.enabled: true
        layer.effect: DelegateShadow {}
    }
}

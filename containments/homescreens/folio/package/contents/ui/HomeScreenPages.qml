// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    required property var homeScreenState

    Repeater {
        model: Folio.PageListModel

        delegate: HomeScreenPage {
            id: homeScreenPage
            pageModel: model.delegate

            anchors.fill: root

            transform: Translate {
                x: root.width * index + homeScreenState.pageViewX
            }
        }
    }
}

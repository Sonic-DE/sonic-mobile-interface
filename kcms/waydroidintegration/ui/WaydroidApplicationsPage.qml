/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.private.mobileshell.waydroidintegrationplugin as AIP

KCM.SimpleKCM {
    id: root

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    title: i18n("Waydroid applications")

    FormCard.FormCard {
        ListView {
            interactive: false

            model: AIP.WaydroidState.applicationListModel

            delegate: FormCard.AbstractFormDelegate {
                id: appDelegate

                width: ListView.view.width

                background: null
                contentItem: RowLayout {
                    QQC2.Label {
                        Layout.fillWidth: true
                        text: model.name
                        elide: Text.ElideRight
                    }

                    // QQC2.ToolButton {
                    //     display: QQC2.AbstractButton.IconOnly
                    //     text: qsDelegate.isEnabled ? i18nc("@action:button", "Hide") : i18nc("@action:button", "Show")
                    //     icon.name: qsDelegate.isEnabled ? "hide_table_row" : "show_table_row"
                    //     onClicked: qsDelegate.isEnabled ? savedQuickSettings.disableQS(model.index) : savedQuickSettings.enableQS(model.index)

                    //     QQC2.ToolTip.visible: hovered
                    //     QQC2.ToolTip.text: text
                    //     QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    // }
                }
            }

            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight
        }
    }
}

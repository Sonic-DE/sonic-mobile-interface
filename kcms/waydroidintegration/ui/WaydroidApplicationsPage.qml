/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import QtQuick.Dialogs

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.waydroidintegrationplugin as AIP

KCM.SimpleKCM {
    id: root

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    title: i18n("Waydroid applications")

    actions: [
        Kirigami.Action {
            text: i18nc("@action:button", "Install APK")
            icon.name: "list-add"

            onTriggered: fileDialog.open()
        }
    ]

    Connections {
        target: AIP.WaydroidState.applicationListModel

        function onErrorOccurred(error: string): void {
            errorInlineMessage.text = error
            errorInlineMessage.visible = true
        }
    }

    FormCard.FormCard {
        Kirigami.InlineMessage {
            id: errorInlineMessage
            visible: false
            Layout.fillWidth: true
            type: Kirigami.MessageType.Error
            showCloseButton: true
        }

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

                    QQC2.ToolButton {
                        display: QQC2.AbstractButton.IconOnly
                        text: i18nc("@action:button", "Delete the application")
                        icon.name: "usermenu-delete"

                        onClicked: AIP.WaydroidState.applicationListModel.deleteApplication(model.id)

                        QQC2.ToolTip.visible: hovered
                        QQC2.ToolTip.text: text
                        QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }
                }
            }

            Layout.fillWidth: true
            Layout.preferredHeight: contentHeight
        }
    }

    FileDialog {
        id: fileDialog
        nameFilters: [ "APK files (*.apk)" ]

        onAccepted: AIP.WaydroidState.applicationListModel.installApk(selectedFile)
    }
}

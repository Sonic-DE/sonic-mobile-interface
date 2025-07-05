/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.androidintegrationplugin as AIP

ColumnLayout {
    id: root

    FormCard.FormHeader {
        title: i18n("General information")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: `${i18n("IP address:")} ${AIP.WaydroidState.ipAddress}`
        }
    }

    FormCard.FormHeader {
        title: i18n("Waydroid props configuration")
    }

    FormCard.FormCard {
        id: infoMessage
        visible: false

        Kirigami.Theme.inherit: false
        Kirigami.Theme.backgroundColor: root.Kirigami.Theme.neutralBackgroundColor

        FormCard.FormTextDelegate {
            text: i18n("May require restarting the waydroid session to apply")
            textItem.wrapMode: Text.WordWrap
            icon.name: "dialog-warning"
        }
    }

    FormCard.FormCard {
        FormCard.FormSwitchDelegate {
            id: multiWindows
            text: i18n("Multi Windows")
            description: i18n("Enables/Disables window integration with the desktop")
            checked: AIP.WaydroidState.multiWindows
            onToggled: {
                AIP.WaydroidState.multiWindows = checked
                infoMessage.visible = true
            }
        }

        FormCard.FormDelegateSeparator { above: multiWindows; below: suspend }

        FormCard.FormSwitchDelegate {
            id: suspend
            text: i18n("Suspend")
            description: i18n("Let the Waydroid container sleep (after the display timeout) when no apps are active")
            checked: AIP.WaydroidState.suspend
            onToggled: {
                AIP.WaydroidState.suspend = checked
                infoMessage.visible = true
            }
        }

        FormCard.FormDelegateSeparator { above: suspend; below: uevent }

        FormCard.FormSwitchDelegate {
            id: uevent
            text: i18n("UEvent")
            description: i18n("Allow android direct access to hotplugged devices")
            checked: AIP.WaydroidState.uevent
            onToggled: {
                AIP.WaydroidState.uevent = checked
                infoMessage.visible = true
            }
        }
    }

    PC3.Button {
        text: i18n("Stop Waydroid session")
        Layout.alignment: Qt.AlignHCenter

        onClicked: AIP.WaydroidState.stopSession()
    }
}
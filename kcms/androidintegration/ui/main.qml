/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.androidintegrationplugin as AIP

KCM.SimpleKCM {
    id: root

    title: i18n("Android Integration")

    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0

    ColumnLayout {
        visible: AIP.WaydroidState.status == AIP.WaydroidState.NotSupported
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing

        QQC2.Label {
            text: i18n("Waydroid is not installed")
        }

        PC3.Button {
            text: i18n("Check installation")
            onClicked: AIP.WaydroidState.checkSupports()
        }
    }

    ColumnLayout {
        visible: AIP.WaydroidState.status == AIP.WaydroidState.NotInitialized

        FormCard.FormHeader {
            title: i18n("Configuration")
        }

        FormCard.FormCard {
            QQC2.ComboBox {
                id: systemType
                textRole: "text"
                Kirigami.FormData.label: i18n("System type")
                model: [{"text": "Vanilla", "value": AIP.WaydroidState.Vanilla},
                        {"text": "FOSS", "value": AIP.WaydroidState.Foss},
                        {"text": "GAPPS", "value": AIP.WaydroidState.Gaps}]
            }

            FormCard.FormDelegateSeparator { above: systemType; below: romType }

            QQC2.ComboBox {
                id: romType
                textRole: "text"
                Kirigami.FormData.label: i18n("ROM type")
                model: [{"text": "Lineage", "value": AIP.WaydroidState.Lineage},
                        {"text": "Bliss", "value": AIP.WaydroidState.Bliss}]
            }

            PC3.Button {
                text: i18n("Configure waydroid")
                onClicked: {
                    const selectedSystemType = systemType.model[systemType.currentIndex].value
                    const selectedRomType = romType.model[romType.currentIndex].value
                    console.log("System type: " + selectedSystemType)
                    console.log("ROM type: " + selectedRomType)
                    // AIP.WaydroidState.initialize(selectedSystemType, selectedRomType)
                }
            }
        }
    }

    ColumnLayout {
        visible: AIP.WaydroidState.status == AIP.WaydroidState.Initialiazing
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing

        PC3.BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            implicitHeight: Kirigami.Units.iconSizes.huge
            implicitWidth: Kirigami.Units.iconSizes.huge

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
        }

        QQC2.Label {
            text: i18n("Waydroid is initializing.\nIt can take a few minutes.")
        }
    }
}
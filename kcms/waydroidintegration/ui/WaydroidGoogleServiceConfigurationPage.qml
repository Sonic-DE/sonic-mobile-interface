/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.waydroidintegrationplugin as AIP

FormCard.FormCardPage {
    id: root

    title: i18n("Google services configuration")

    Component.onCompleted: {
        if (AIP.WaydroidState.androidId === "") {
            AIP.WaydroidState.refreshAndroidId()
        }
    }

    WaydroidLoader {
        visible: AIP.WaydroidState.androidId === ""
        text: i18n("We fetching your Android ID.\nIt can take a few seconds.")
    }

    ColumnLayout {
        visible: AIP.WaydroidState.androidId !== ""
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent
        anchors.leftMargin: Kirigami.Units.largeSpacing
        anchors.right: parent
        anchors.rightMargin: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        QQC2.Label {
            text: i18n("When launching waydroid with GAPPS for the first time you will be notified that the device is not certified for Google Play Protect. To self certify your device, paste the Android ID on the field on the website.")
            wrapMode: TextEdit.Wrap
            Layout.fillWidth: true
        }

        PC3.Button {
            text: i18n('Copy Android ID and open the website')
            icon.name: 'edit-copy-symbolic'
            onClicked: {
                AIP.WaydroidState.copyToClipboard(AIP.WaydroidState.androidId)
                Qt.openUrlExternally("https://www.google.com/android/uncertified")
            }
        }
    }
}
/*
 * SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.plasma.components 3.0 as PC3

ColumnLayout {
    id: root
    property string text
    property real downloaded: 0.0
    property real total: 0.0
    property real speed: 0.0

    anchors.centerIn: parent
    spacing: Kirigami.Units.largeSpacing

    QQC2.ProgressBar {
        from: 0
        value: downloaded
        to: total
        indeterminate: total <= 0.0

        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        text: root.text
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        visible: total > 0.0
        text: `${i18n("Downloaded")} ${downloaded.toFixed(2)} ${i18n("MB")}`
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        visible: total > 0.0
        text: `${i18n("Total")} ${total.toFixed(2)} ${i18n("MB")}`
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter
    }

    QQC2.Label {
        visible: total > 0.0
        text: `${i18n("Speed")} ${speed.toFixed(2)} ${i18n("Kbps")}`
        horizontalAlignment: Text.AlignHCenter
        Layout.alignment: Qt.AlignHCenter
    }
}
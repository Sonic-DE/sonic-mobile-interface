/*
 *  SPDX-FileCopyrightText: 2026 SonicDE Contributors
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

pragma Singleton

import QtQuick

// FreeBSD has no BlueZ, so Bluetooth is never available: BluetoothInfo.qml's properties, off.
QtObject {
    readonly property bool isVisible: false
    readonly property string icon: "network-bluetooth"
    readonly property bool deviceConnected: false

    function updateStatus() {
    }
}

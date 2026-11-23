/*
 *  SPDX-FileCopyrightText: 2026 SonicDE Contributors
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

pragma Singleton

import QtQuick

// FreeBSD has no ModemManager, so there is never a modem: SignalStrengthInfo.qml's properties, empty.
QtObject {
    readonly property string icon: "network-mobile-0"
    readonly property string label: ""
    readonly property bool showIndicator: false
}

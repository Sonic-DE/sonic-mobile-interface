// SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15

import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS
import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.plasma.private.mobileshell.waydroidintegrationplugin as AIP

QS.QuickSetting {
    text: i18n("Waydroid")
    status: AIP.WaydroidState.sessionStatus == AIP.WaydroidState.SessionRunning ? i18n("Running") : i18n("Stopped")
    icon: "folder-android-symbolic"
    settingsCommand: "plasma-open-settings kcm_waydroidintegration"

    enabled: AIP.WaydroidState.sessionStatus == AIP.WaydroidState.SessionRunning
    available: AIP.WaydroidState.status == AIP.WaydroidState.Initialized

    function toggle() {
        if (AIP.WaydroidState.sessionStatus == AIP.WaydroidState.SessionRunning) {
            AIP.WaydroidState.stopSession()
        } else {
            AIP.WaydroidState.startSession()
        }
    }
}

// SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15

import org.kde.plasma.quicksetting.kscreenosd 1.0
import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings

QS.QuickSetting {
    id: kscreenosd_qs
    text: i18n("Display Config")
    icon: "osd-duplicate"
    settingsCommand: "plasma-open-settings kcm_kscreen"
    status: i18nc("kscreen osd quicksetting", "Tap to set up")
    enabled: false
    available: true

    function toggle() {
        console.log("Showing KScreen OSD");
        KScreenOSDUtil.showKScreenOSD();
    }

    Connections {
        target: KScreenOSDUtil
        function onOutputsChanged() {
            console.log("KScreen OSD connection changed, outputs: " + KScreenOSDUtil.outputs);
            kscreenosd_qs.available = KScreenOSDUtil.outputs > 1;
            console.log("KScreen OSD convergenceModeEnabled: " + (KScreenOSDUtil.outputs > 1 ? "true" : "false"));
            ShellSettings.Settings.convergenceModeEnabled = KScreenOSDUtil.outputs > 1;

        }
    }
}

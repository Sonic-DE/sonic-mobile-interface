// SPDX-FileCopyrightText: 2025 Sebastian Kŭgler <sebas@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick

import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.plasma.private.mobileshell.state as MobileShellState
import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS

QS.QuickSetting {
    id: lts_root
    text: i18n("Lock Touchscreen")
    icon: "input-touchscreen"
    //status: i18n("%1%", MobileShell.AudioInfo.volumeValue)
    enabled: true
    //settingsCommand: "plasma-open-settings kcm_pulseaudio"

    property var lts_component: null
    property var lts_object: null

    function toggle() {
        MobileShellState.ShellDBusClient.closeActionDrawer();
        console.log("Locking the touchscreen now...");
        lts_component = Qt.createComponent("LockTouchScreen.qml");
        if (lts_component.status == Component.Ready) {
            console.log("not yet finished")
            finishCreation();
        } else {
            lts_component.statusChanged.connect(finishCreation);
        }
    }

    function finishCreation() {
        if (lts_component.status == Component.Ready) {
            lts_object = lts_component.createObject(lts_root, {});
            if (lts_object == null) {
                // Error Handling
                console.log("Error creating object");
            }
        } else if (lts_component.status == Component.Error) {
            // Error Handling
            console.log("Error loading component:", lts_component.errorString());
        }
    }
}

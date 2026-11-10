// SPDX-FileCopyrightText: 2022-2025 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window

import org.kde.plasma.private.mobileshell.state as MobileShellState
import org.kde.plasma.quicksetting.record
import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS

QS.QuickSetting {
    id: root

    text: RecordUtil.quickSettingText
    status: RecordUtil.quickSettingStatus
    icon: "camera-video-symbolic"
    enabled: RecordUtil.isRecording
    available: true

    function toggle() {
        if (RecordUtil.isRecording) {
            RecordUtil.stopRecording();
        } else if (RecordUtil.startRecording(Screen.name)) {
            MobileShellState.ShellDBusClient.closeActionDrawer();
        }
    }
}

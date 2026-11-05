/*
 * SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@kde.org>
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS
import org.kde.plasma.private.mobileshell as MobileShell

// Sonic Keyboard opens by itself in each application when text input gets focus, so there is nothing
// to switch on or off here: this opens its settings.
QS.QuickSetting {
    text: i18n("Virtual Keyboard")
    icon: "input-keyboard-virtual"
    status: i18n("Tap to open settings")
    enabled: false
    settingsCommand: "plasma-open-settings kcm_plasmakeyboard"

    function toggle() {
        MobileShell.ShellUtil.executeCommand("plasma-open-settings kcm_plasmakeyboard");
    }
}

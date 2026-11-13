// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick

import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings
import org.kde.plasma.private.mobileshell.quicksettingsplugin as QS

QS.QuickSetting {
    text: i18n("Dark Mode")
    icon: "contrast"
    enabled: ShellSettings.Settings.darkMode

    function toggle() {
        ShellSettings.Settings.darkMode = !ShellSettings.Settings.darkMode;
    }
}

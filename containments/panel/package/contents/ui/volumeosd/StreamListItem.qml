/*
    SPDX-FileCopyrightText: 2014-2015 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.0

import org.kde.plasma.private.volume 0.1

ListItemBase {
    id: item

    property QtObject devicesModel

    label: {
        if (Client && Client.name) {
            return Client.name;
        }
        if (Name) {
            return Name;
        }
        return i18n("Stream name not found");
    }

    listIcon: {
        if (IconName.length !== 0) {
            return IconName
        }

        if (item.type === "source-output") {
            return "audio-input-microphone"
        }

        return "audio-volume-high"
    }
    iconUsesPlasmaTheme: false
}

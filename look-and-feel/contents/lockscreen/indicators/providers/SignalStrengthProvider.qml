/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *  SPDX-FileCopyrightText: 2021 Tobias Fella <fella@posteo.de>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.1

import org.kde.plasma.mm 1.0

QtObject {

    SignalIndicator {
        id: signalIndicator
    }

    property string icon: signalIndicator.strength == 100 ? "network-mobile-100"
                        : signalIndicator.strength >= 80 ? "network-mobile-80"
                        : signalIndicator.strength >= 60 ? "network-mobile-60"
                        : signalIndicator.strength >= 40 ? "network-mobile-40"
                        : signalIndicator.strength >= 20 ? "network-mobile-20"
                        : "network-mobile-0"

    property string label: signalIndicator.simLocked ? i18n("Sim locked") : signalIndicator.name
}


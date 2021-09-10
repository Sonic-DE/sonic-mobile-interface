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
    property string icon: SignalIndicator.strength == 100 ? "network-mobile-100"
                        : SignalIndicator.strength >= 80 ? "network-mobile-80"
                        : SignalIndicator.strength >= 60 ? "network-mobile-60"
                        : SignalIndicator.strength >= 40 ? "network-mobile-40"
                        : SignalIndicator.strength >= 20 ? "network-mobile-20"
                        : "network-mobile-0"

    property string label: SignalIndicator.simLocked ? i18n("Sim locked") : SignalIndicator.name
}


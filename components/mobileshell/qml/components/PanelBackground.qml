/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.4
import QtGraphicalEffects 1.12

import org.kde.plasma.core 2.0 as PlasmaCore

BaseItem {
    id: root

    leftPadding: frameSvg.margins.left
    topPadding: frameSvg.margins.top
    rightPadding: frameSvg.margins.right
    bottomPadding: frameSvg.margins.bottom

    background: PlasmaCore.FrameSvgItem {
        id: frameSvg
        imagePath: "widgets/background"
    }
}


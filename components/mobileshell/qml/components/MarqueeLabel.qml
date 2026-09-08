// SPDX-FileCopyrightText: 2022 Yari Polla <skilvingr@gmail.com>
// SPDX-FileCopyrightText: 2025 Micah Stanley <stanleymicah@proton.me>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * This is a simple marquee (flowing) label based on PlasmaComponents Label.
 */

MultiEffect {
    id: root
    height: row.height

    // label values
    required property string inputText
    property font font
    property var textFormat: Text.RichText
    // properties for the marquee label scroll speed and wait duration
    readonly property real scrollSpeed: 0.025
    readonly property int waitDuration: 2000

    readonly property string filteredText: inputText.replace(/\n/g, ' ') // remove new line characters
    readonly property bool charactersOverflowing: txtMeter.advanceWidth > root.width // true when text is overflowing

    // update animation values and text positions whenever the label overflows or changes
    onFilteredTextChanged: if (root.charactersOverflowing) { textAnimationLoop.restart() }
    onCharactersOverflowingChanged: if (charactersOverflowing) { row.scrollPosition = 0 }

    maskEnabled: true
    source: ShaderEffectSource {
        sourceItem: Item {
            id: rowContaner
            anchors.fill: parent
            height: row.height

            // use two identical labels for scrolling so we can give the illusion of infinite scrolling
            RowLayout {
                id: row
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                property real scrollPosition: 0

                transform: [
                    Translate {
                        x: row.scrollPosition
                    }
                ]

                spacing: 32

                PlasmaComponents.Label {
                    id: label
                    font: root.font
                    textFormat: root.textFormat
                    text: root.filteredText

                    Layout.alignment: Qt.AlignLeft

                    TextMetrics {
                        id: txtMeter
                        font: root.font
                        text: root.filteredText
                    }
                }

                PlasmaComponents.Label {
                    // hide this label when the text is not overflowing so the user never sees both labels
                    visible: textAnimationLoop.running
                    font: root.font
                    textFormat: root.textFormat
                    text: root.filteredText

                    Layout.alignment: Qt.AlignLeft
                }
            }
        }
        hideSource: true
        live: true
    }

    maskSource: Rectangle {
        id: mask
        width: root.width
        height: root.height
        layer.enabled: true

        property real gradientPct: (Kirigami.Units.gridUnit * 0.35) / root.width

        gradient: Gradient {
            orientation: Gradient.Horizontal

            GradientStop { position: 0; color: row.scrollPosition == 0 || row.scrollPosition < -txtMeter.advanceWidth ? 'white' : 'transparent' }
            GradientStop { position: 0 + mask.gradientPct; color: 'white' }
            GradientStop { position: 1.0 - mask.gradientPct; color: 'white' }
            GradientStop { position: 1.0; color: 'transparent' }
        }
    }

    // if the label is overflowing, this animation in a loop smoothly scrolling thought the text
    SequentialAnimation {
        id: textAnimationLoop
        running: root.charactersOverflowing && root.visible
        onRunningChanged: row.scrollPosition = 0
        loops: Animation.Infinite
        PauseAnimation { duration: root.waitDuration }
        NumberAnimation { target: row; property: "scrollPosition"; from: 0; to: -txtMeter.advanceWidth - row.spacing; duration: (txtMeter.advanceWidth + row.spacing) / root.scrollSpeed }
    }
}

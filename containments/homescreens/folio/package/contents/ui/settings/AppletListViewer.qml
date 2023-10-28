// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.plasmoid
import org.kde.plasma.private.shell 2.0
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3

import '../delegate'

MouseArea {
    id: root

    signal requestClose()
    onClicked: root.requestClose()

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.7)
    }

    Kirigami.Heading {
        id: heading
        level: 1
        text: i18n("Widgets")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Kirigami.Units.gridUnit * 3
    }

    GridView {
        id: gridView
        clip: true

        anchors.top: heading.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        model: widgetExplorer.widgetsModel
        cellWidth: width / 3 // TODO
        cellHeight: cellWidth + Kirigami.Units.gridUnit * 3

        delegate: Item {
            id: delegate
            width: gridView.cellWidth
            height: gridView.cellHeight

            readonly property string pluginName: model.pluginName

            ColumnLayout {
                anchors.fill: parent

                Item {
                    id: iconWidget
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    Layout.preferredHeight: Kirigami.Units.iconSizes.large
                    Layout.preferredWidth: Kirigami.Units.iconSizes.large

                    Kirigami.Icon {
                        anchors.centerIn: parent
                        source: model.decoration
                        visible: model.screenshot == ""
                        implicitWidth: Kirigami.Units.iconSizes.large
                        implicitHeight: Kirigami.Units.iconSizes.large
                    }
                    Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                        source: model.screenshot
                        width: Kirigami.Units.iconSizes.large
                        height: Kirigami.Units.iconSizes.large
                    }
                }

                Kirigami.Heading {
                    id: heading
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    level: 4
                    text: model.name
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                    horizontalAlignment: Text.AlignHCenter
                }

                PC3.Label {
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    // otherwise causes binding loop due to the way the Plasma sets the height
                    height: implicitHeight
                    text: model.description
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                    maximumLineCount: heading.lineCount === 1 ? 3 : 2
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    WidgetExplorer {
        id: widgetExplorer
        containment: Plasmoid
    }
}

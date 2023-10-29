// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls as QQC2

import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import org.kde.plasma.private.shell 2.0
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.plasma.components 3.0 as PC3

import '../delegate'
import '../private'

MouseArea {
    id: root

    property var homeScreen

    signal requestClose()
    onClicked: root.requestClose()

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.7)
    }

    PC3.Label {
        id: heading
        color: 'white'
        text: i18n("Widgets")
        font.weight: Font.Bold
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Kirigami.Units.gridUnit * 3 + root.homeScreen.topMargin
    }

    GridView {
        id: gridView
        clip: true
        reuseItems: true

        opacity: 0 // we display with the opacity gradient below

        anchors.top: heading.bottom
        anchors.topMargin: Kirigami.Units.gridUnit
        anchors.left: parent.left
        anchors.leftMargin: root.homeScreen.leftMargin
        anchors.right: parent.right
        anchors.rightMargin: root.homeScreen.rightMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.homeScreen.bottomMargin

        model: widgetExplorer.widgetsModel
        cellWidth: (width - leftMargin - rightMargin) / 3 // TODO
        cellHeight: cellWidth + Kirigami.Units.gridUnit * 3

        leftMargin: Kirigami.Units.gridUnit
        rightMargin: Kirigami.Units.gridUnit

        delegate: MouseArea {
            id: delegate
            width: gridView.cellWidth
            height: gridView.cellHeight

            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true

            readonly property string pluginName: model.pluginName

            onClicked: {
                // TODO drag and drop
                Folio.PageListModel.createWidgetDelegate(pluginName);
            }

            Rectangle {
                id: background
                color: Qt.rgba(255, 255, 255, 0.3)
                visible: delegate.containsMouse
                radius: Kirigami.Units.smallSpacing
                anchors.fill: parent
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing

                Item {
                    id: iconWidget
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    Layout.preferredHeight: Kirigami.Units.iconSizes.large
                    Layout.preferredWidth: Kirigami.Units.iconSizes.large
                    Layout.alignment: Qt.AlignBottom

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

                PC3.Label {
                    id: heading
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    Layout.alignment: Qt.AlignCenter
                    text: model.name
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                    horizontalAlignment: Text.AlignHCenter
                    font.weight: Font.Bold
                }

                PC3.Label {
                    Layout.fillWidth: true
                    Layout.maximumWidth: delegate.width
                    Layout.alignment: Qt.AlignTop
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

    // opacity gradient at grid edges
    FlickableOpacityGradient {
        anchors.fill: gridView
        flickable: gridView
    }

    WidgetExplorer {
        id: widgetExplorer
        containment: Plasmoid
    }
}

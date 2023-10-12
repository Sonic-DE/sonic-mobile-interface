// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Folio.DelegateTouchArea {
    id: root

    property Folio.FolioApplicationFolder folder

    property bool inFolderTitleEditMode: false

    // TODO FILL FOLDER page WIDTH AND HEIGHT FIELDS

    onClicked: {
        close();
    }

    function close() {
        Folio.HomeScreenState.closeFolder();
    }

    MobileShell.BaseItem {
        id: titleText
        width: root.width

        anchors.bottom: folderBackground.bottom
        anchors.bottomMargin: Kirigami.Units.gridUnit

        background: Rectangle {
            color: 'transparent'
            TapHandler {
                onTapped: root.close()
            }
        }

        Component {
            id: folderTitleEdit

            TextEdit {
                text: root.folder ? root.folder.name : ""
                color: "white"
                selectByMouse: true
                wrapMode: TextEdit.Wrap

                Component.onCompleted: forceActiveFocus()

                font.weight: Font.Bold
                font.pointSize: 18

                layer.enabled: true
                layer.effect: MobileShell.TextDropShadow {}

                onTextChanged: {
                    if (text.includes('\n')) {
                        // exit text edit mode when new line is entered
                        root.inFolderTitleEditMode = false;
                    } else {
                        root.folder.name = text;
                    }
                }
                onEditingFinished: root.inFolderTitleEditMode = false
            }
        }

        Component {
            id: folderTitleLabel

            QQC2.Label {
                text: root.folder ? root.folder.name : ""
                color: "white"
                style: Text.Normal
                styleColor: "transparent"
                horizontalAlignment: Text.AlignHCenter
                textFormat: Text.MarkdownText

                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 2

                font.weight: Font.Bold
                font.pointSize: 18

                layer.enabled: true
                layer.effect: MobileShell.TextDropShadow {}

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.inFolderTitleEditMode = true
                }
            }
        }

        // folder title
        contentItem: Loader {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            sourceComponent: root.inFolderTitleEditMode ? folderTitleEdit : folderTitleLabel
        }
    }

    Rectangle {
        id: folderBackground
        color: Qt.rgba(255, 255, 255, 0.3)
        radius: Kirigami.Units.gridUnit

        anchors.centerIn: parent

        property real length: Math.min(root.width * 0.7, root.height * 0.7)
        width: length
        height: length
    }
}

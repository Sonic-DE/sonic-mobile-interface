// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import "steps"

import initialstart 1.0 as InitialStart

Kirigami.Page {
    id: root

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    property bool showingLanding: true

    readonly property bool onFinalPage: swipeView.currentIndex === (swipeView.count - 1)

    function finishFinalPage() {
        InitialStart.Wizard.wizardFinished();
        applicationWindow().close();
    }

    // top status bar
    MobileShell.StatusBar {
        id: statusBar
        z: 1

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: Kirigami.Units.gridUnit * 1.25

        colorGroup: PlasmaCore.Theme.ComplementaryColorGroup
        backgroundColor: "transparent"

        showSecondRow: false
        showDropShadow: true
        showTime: true
        disableSystemTray: true // prevent SIGABRT, since loading the system tray leads to bad... things
    }

    LandingComponent {
        id: landingComponent
        anchors.fill: parent

        onRequestNextPage: {
            root.showingLanding = false;
            stepHeading.changeText(swipeView.currentItem.name);
        }
    }

    Item {
        id: stepsComponent
        anchors.fill: parent

        // animation when we switch to step stage
        opacity: root.showingLanding ? 0 : 1
        property real translateY: root.showingLanding ? overlaySteps.height : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 1000
                easing.type: Easing.OutExpo
            }
        }

        Behavior on translateY {
            NumberAnimation {
                duration: 1000
                easing.type: Easing.OutExpo
            }
        }

        transform: Translate { y: stepsComponent.translateY }

        // heading for all the wizard steps
        Label {
            id: stepHeading
            opacity: 0
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 18

            anchors.left: parent.left
            anchors.leftMargin: Kirigami.Units.gridUnit
            anchors.right: parent.right
            anchors.rightMargin: Kirigami.Units.gridUnit
            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.height * 0.7 + Kirigami.Units.gridUnit

            property string toText

            function changeText(text) {
                toText = text;
                toHidden.restart();
            }

            NumberAnimation on opacity {
                id: toHidden
                duration: 200
                to: 0
                onFinished: {
                    stepHeading.text = stepHeading.toText;
                    toShown.restart();
                }
            }

            NumberAnimation on opacity {
                id: toShown
                duration: 200
                to: 1
            }
        }

        Rectangle {
            id: overlaySteps

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor

            anchors.fill: parent
            anchors.topMargin: root.height * 0.3

            // all steps are in a swipeview
            SwipeView {
                id: swipeView
                anchors.fill: parent
                anchors.bottomMargin: stepFooter.implicitHeight
                currentIndex: 0
                interactive: false

                function requestNextPage() {
                    currentIndex++;
                    stepHeading.changeText(currentItem.name);
                }

                function requestPreviousPage() {
                    if (currentIndex === 0) {
                        root.showingLanding = true;
                        landingComponent.returnToLanding();
                    } else {
                        currentIndex--;
                        stepHeading.changeText(currentItem.name);
                    }
                }

                // setup steps
                DisplayScalingStep { height: swipeView.height; width: swipeView.height }
                CellularStep { height: swipeView.height; width: swipeView.height }
                FinalStep { height: swipeView.height; width: swipeView.height }
            }

            // bottom footer
            RowLayout {
                id: stepFooter
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                Button {
                    Layout.alignment: Qt.AlignLeft
                    Layout.leftMargin: Kirigami.Units.gridUnit
                    Layout.bottomMargin: Kirigami.Units.gridUnit

                    topPadding: Kirigami.Units.largeSpacing
                    bottomPadding: Kirigami.Units.largeSpacing
                    leftPadding: Kirigami.Units.gridUnit
                    rightPadding: Kirigami.Units.gridUnit

                    text: i18n("Back")
                    icon.name: "arrow-left"

                    onClicked: swipeView.requestPreviousPage()
                }

                Item {}

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: Kirigami.Units.gridUnit
                    Layout.bottomMargin: Kirigami.Units.gridUnit

                    topPadding: Kirigami.Units.largeSpacing
                    bottomPadding: Kirigami.Units.largeSpacing
                    leftPadding: Kirigami.Units.gridUnit
                    rightPadding: Kirigami.Units.gridUnit

                    visible: !root.onFinalPage
                    text: i18n("Next")
                    icon.name: "arrow-right"

                    onClicked: swipeView.requestNextPage();
                }

                Button {
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: Kirigami.Units.gridUnit
                    Layout.bottomMargin: Kirigami.Units.gridUnit

                    topPadding: Kirigami.Units.largeSpacing
                    bottomPadding: Kirigami.Units.largeSpacing
                    leftPadding: Kirigami.Units.gridUnit
                    rightPadding: Kirigami.Units.gridUnit

                    visible: root.onFinalPage
                    text: i18n("Finish")
                    icon.name: "dialog-ok"

                    onClicked: root.finishFinalPage();
                }
            }
        }
    }
}


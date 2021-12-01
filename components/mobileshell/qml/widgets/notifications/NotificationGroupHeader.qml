/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * 
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.8
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.notificationmanager 1.0 as NotificationManager

import org.kde.kcoreaddons 1.0 as KCoreAddons

import "util.js" as Util

RowLayout {
    id: notificationHeading
    property int notificationType

    property var applicationIconSource
    property string applicationName
    property string originName

    property string configureActionLabel

    property bool configurable: false
    property bool dismissable: false
    property bool dismissed
    property string closeButtonTooltip: i18n("Close")
    property bool closable

    property var time
    property PlasmaCore.DataSource timeSource

    property int jobState
    property QtObject jobDetails

    property real timeout: 5000
    property real remainingTime: 0

    signal configureClicked
    signal dismissClicked
    signal closeClicked

    spacing: PlasmaCore.Units.smallSpacing
    Layout.preferredHeight: Math.max(applicationNameLabel.implicitHeight, PlasmaCore.Units.iconSizes.small)

    PlasmaCore.IconItem {
        id: applicationIconItem
        Layout.topMargin: PlasmaCore.Units.smallSpacing
        Layout.bottomMargin: PlasmaCore.Units.smallSpacing
        Layout.preferredWidth: PlasmaCore.Units.iconSizes.small
        Layout.preferredHeight: PlasmaCore.Units.iconSizes.small
        source: notificationHeading.applicationIconSource
        usesPlasmaTheme: false
        visible: valid
    }

    PlasmaComponents.Label {
        id: applicationNameLabel
        Layout.leftMargin: PlasmaCore.Units.smallSpacing
        Layout.fillWidth: true
        opacity: 0.9
        textFormat: Text.PlainText
        elide: Text.ElideLeft
        text: notificationHeading.applicationName + (notificationHeading.originName ? " · " + notificationHeading.originName : "")
    }

    Item {
        id: spacer
        Layout.fillWidth: true
    }

    NotificationTimeText {
        notificationType: notificationHeading.notificationType
        jobState: notificationHeading.jobState
        jobDetails: notificationHeading.jobDetails
        
        time: notificationHeading.time
        timeSource: notificationHeading.timeSource
    }

    // TODO re-implement with gestures
//     RowLayout {
//         id: headerButtonsRow
//         spacing: 0
// 
//         PlasmaComponents3.ToolButton {
//             id: configureButton
//             icon.name: "configure"
//             visible: configurable
//             onClicked: notificationHeading.configureClicked()
// 
//             PlasmaComponents3.ToolTip {
//                 text: notificationHeading.configureActionLabel || i18nd("plasma_applet_org.kde.plasma.notifications", "Configure")
//             }
//         }
// 
//         PlasmaComponents3.ToolButton {
//             id: dismissButton
//             icon.name: notificationHeading.dismissed ? "window-restore" : "window-minimize"
//             visible: dismissable
//             onClicked: notificationHeading.dismissClicked()
// 
//             PlasmaComponents3.ToolTip {
//                 text: notificationHeading.dismissed
//                       ? i18ndc("plasma_applet_org.kde.plasma.notifications", "Opposite of minimize", "Restore")
//                       : i18nd("plasma_applet_org.kde.plasma.notifications", "Minimize")
//             }
//         }
// 
//         PlasmaComponents3.ToolButton {
//             id: closeButton
//             visible: closable
//             icon.name: "window-close"
//             onClicked: notificationHeading.closeClicked()
// 
//             PlasmaComponents3.ToolTip {
//                 id: closeButtonToolTip
//                 text: closeButtonTooltip
//             }
// 
//             Charts.PieChart {
//                 id: chart
//                 anchors.fill: parent
//                 anchors.margins: PlasmaCore.Units.smallSpacing + Math.max(Math.floor(PlasmaCore.Units.devicePixelRatio), 1)
// 
//                 opacity: (notificationHeading.remainingTime > 0 && notificationHeading.remainingTime < notificationHeading.timeout) ? 1 : 0
//                 Behavior on opacity {
//                     NumberAnimation { duration: PlasmaCore.Units.longDuration }
//                 }
// 
//                 range { from: 0; to: notificationHeading.timeout; automatic: false }
// 
//                 valueSources: Charts.SingleValueSource { value: notificationHeading.remainingTime }
//                 colorSource: Charts.SingleValueSource { value: PlasmaCore.Theme.highlightColor }
// 
//                 thickness: Math.max(Math.floor(PlasmaCore.Units.devicePixelRatio), 1) * 5
// 
//                 transform: Scale { origin.x: chart.width / 2; xScale: -1 }
//             }
//         }
//     }
}

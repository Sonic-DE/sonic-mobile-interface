/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * 
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.8
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.notificationmanager 1.0 as NotificationManager

import org.kde.kirigami 2.12 as Kirigami

import org.kde.kcoreaddons 1.0 as KCoreAddons

import "util.js" as Util

// notification properties are in BaseNotificationItem
BaseNotificationItem {
    id: notificationItem
    implicitHeight: mainCard.implicitHeight
    
    // notification heading for groups with one element
    NotificationGroupHeader {
        id: notificationHeading
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        PlasmaCore.ColorScope.colorGroup: PlasmaCore.Theme.HeaderColorGroup
        PlasmaCore.ColorScope.inherit: false

        visible: !notificationItem.inGroup
        height: visible ? implicitHeight : 0

        applicationName: notificationItem.applicationName
        applicationIconSource: notificationItem.applicationIconSource
        originName: notificationItem.originName
        
        notificationType: notificationItem.notificationType
        jobState: notificationItem.jobState
        jobDetails: notificationItem.jobDetails
        
        time: notificationItem.time
        timeSource: notificationItem.timeSource
    }
    
    // notification
    NotificationCard {
        id: mainCard
        anchors.topMargin: notificationHeading.visible ? Kirigami.Units.largeSpacing : 0
        anchors.top: notificationHeading.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        
        swipeGestureEnabled: notificationItem.notificationType != NotificationManager.Notifications.JobType
        onDismissRequested: notificationItem.notificationsModel.close(notificationItem.notificationsModel.index(index, 0));
        
        ColumnLayout {
            id: column
            spacing: 0
            
            RowLayout {
                Layout.fillWidth: true
                Layout.bottomMargin: PlasmaCore.Units.smallSpacing
                
                // notification summary
                PlasmaComponents.Label {
                    id: summaryLabel
                    Layout.fillWidth: true
                    textFormat: Text.PlainText
                    maximumLineCount: 3
                    wrapMode: Text.WordWrap
                    elide: Text.ElideRight
                    text: Util.determineNotificationHeadingText(notificationItem)
                    visible: text !== ""
                    font.weight: Font.DemiBold
                }
                
                // notification timestamp
                NotificationTimeText {
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    notificationType: notificationItem.notificationType
                    jobState: notificationItem.jobState
                    jobDetails: notificationItem.jobDetails
                    
                    time: notificationItem.time
                    timeSource: notificationItem.timeSource
                }
            }
            
            // notification contents
            RowLayout {
                Layout.fillWidth: true
                spacing: PlasmaCore.Units.smallSpacing

                // notification text
                NotificationBodyLabel {
                    id: bodyLabel
                    Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                    Layout.fillWidth: true
                    
                    // HACK RichText does not allow to specify link color and since LineEdit
                    // does not support StyledText, we have to inject some CSS to force the color,
                    // cf. QTBUG-81463 and to some extent QTBUG-80354
                    text: "<style>a { color: " + PlasmaCore.Theme.linkColor + "; }</style>" + notificationItem.body

                    // Cannot do text !== "" because RichText adds some HTML tags even when empty
                    visible: notificationItem.body !== ""
                }
                
                // notification icon
                Item {
                    id: iconContainer
                    Layout.preferredWidth: PlasmaCore.Units.iconSizes.large
                    Layout.preferredHeight: PlasmaCore.Units.iconSizes.large
                    Layout.topMargin: PlasmaCore.Units.smallSpacing
                    Layout.bottomMargin: PlasmaCore.Units.smallSpacing

                    visible: iconItem.active

                    PlasmaCore.IconItem {
                        id: iconItem
                        // don't show two identical icons
                        readonly property bool active: valid && source != notificationItem.applicationIconSource
                        anchors.fill: parent
                        usesPlasmaTheme: false
                        smooth: true
                        // don't show a generic "info" icon since this is a notification already
                        source: notificationItem.icon !== "dialog-information" ? notificationItem.icon : ""
                        visible: active
                    }
                }
            }
            
            Item {
                id: actionContainer
                Layout.fillWidth: true
                Layout.preferredHeight: Math.max(actionFlow.implicitHeight, replyLoader.height)
                visible: actionRepeater.count > 0

                // Notification actions
                Flow { // it's a Flow so it can wrap if too long
                    id: actionFlow
                    width: parent.width
                    spacing: PlasmaCore.Units.smallSpacing
                    layoutDirection: Qt.RightToLeft
                    enabled: !replyLoader.active
                    opacity: replyLoader.active ? 0 : 1
                    Behavior on opacity {
                        NumberAnimation {
                            duration: PlasmaCore.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    Repeater {
                        id: actionRepeater

                        model: {
                            var buttons = [];
                            var actionNames = (notificationItem.actionNames || []);
                            var actionLabels = (notificationItem.actionLabels || []);
                            // HACK We want the actions to be right-aligned but Flow also reverses
                            for (var i = actionNames.length - 1; i >= 0; --i) {
                                buttons.push({
                                    actionName: actionNames[i],
                                    label: actionLabels[i]
                                });
                            }
                            
                            if (notificationItem.hasReplyAction) {
                                buttons.unshift({
                                    actionName: "inline-reply",
                                    label: notificationItem.replyActionLabel || i18nc("Reply to message", "Reply")
                                });
                            }
                            
                            return buttons;
                        }

                        PlasmaComponents.ToolButton {
                            flat: false
                            text: modelData.label || ""

                            onClicked: {
                                if (modelData.actionName === "inline-reply") {
                                    replyLoader.beginReply();
                                    return;
                                }
                                notificationItem.actionInvoked(modelData.actionName);
                            }
                        }
                    }
                }
                
                // inline reply field
                Loader {
                    id: replyLoader
                    width: parent.width
                    height: active ? item.implicitHeight : 0
                    
                    // When there is only one action and it is a reply action, show text field right away
                    active: false
                    visible: active
                    opacity: active ? 1 : 0
                    x: active ? 0 : parent.width
                    
                    Behavior on x {
                        NumberAnimation {
                            duration: PlasmaCore.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }
                    Behavior on opacity {
                        NumberAnimation {
                            duration: PlasmaCore.Units.longDuration
                            easing.type: Easing.InOutQuad
                        }
                    }

                    function beginReply() {
                        active = true
                        replyLoader.item.activate();
                    }

                    sourceComponent: NotificationReplyField {
                        placeholderText: notificationItem.replyPlaceholderText
                        buttonIconName: notificationItem.replySubmitButtonIconName
                        buttonText: notificationItem.replySubmitButtonText
                        onReplied: notificationItem.replied(text)
                        
                        replying: replyLoader.active
                        onBeginReplyRequested: replyLoader.beginReply()
                    }
                }
            }
            
            // thumbnails
            Loader {
                id: thumbnailStripLoader
                Layout.topMargin: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                active: notificationItem.urls.length > 0
                visible: active
                sourceComponent: ThumbnailStrip {
                    leftPadding: -thumbnailStripLoader.Layout.leftMargin
                    rightPadding: -thumbnailStripLoader.Layout.rightMargin
                    topPadding: -notificationItem.thumbnailTopPadding
                    bottomPadding: -thumbnailStripLoader.Layout.bottomMargin
                    urls: notificationItem.urls
                    onOpenUrl: notificationItem.openUrl(url)
                    onFileActionInvoked: notificationItem.fileActionInvoked(action)
                }
            }
        }
    }
}

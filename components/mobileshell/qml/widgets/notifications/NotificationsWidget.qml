/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *   SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 *   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 3.0 as PlasmaComponents3

import org.kde.notificationmanager 1.0 as NotificationManager

/**
 * Embeddable notification list widget optimized for mobile and touch.
 * Used on the lockscreen and action drawer.
 */
Item {
    id: root
    
    /**
     * The notification model for the widget.
     */
    property var historyModel
    
    /**
     * The type of notification model used for the widget.
     */
    property int historyModelType
    
    /**
     * The notification model settings for the widget.
     */
    property var notificationSettings
    
    /**
     * Whether invoking notification actions requires authentiation of some sort.
     * 
     * If set to true, any attempted invoking will trigger the unlockRequested() signal.
     * Any consumers can then call the runPendingAction() function if authenticated to proceed
     * executing the notification action.
     */
    property bool actionsRequireUnlock: false
    
    /**
     * Whether the widget has notifications.
     */
    readonly property bool hasNotifications: list.count > 0
    
    readonly property bool doNotDisturbModeEnabled: !isNaN(notificationSettings.notificationsInhibitedUntil)
    
    enum ModelType {
        NotificationsModel, // used in the logged-in shell
        WatchedNotificationsModel // used on the lockscreen
    }
    
    /**
     * Signal emitted when authentication is requested for an action.
     * Listeners should call runPendingAction() if authentication is successful.
     * 
     * Only emitted if actionsRequireUnlock is enabled.
     */
    signal unlockRequested()
    
    /**
     * Emitted when the background is clicked (not a notification or other element).
     */
    signal backgroundClicked()
    
    /**
     * Run pending action that was pending for authentication when unlockRequested() was emitted.
     */
    function runPendingAction() {
        list.pendingNotificationWithAction.runPendingAction();
    }
    
    /**
     * Clears the history of the notification model.
     */
    function clearHistory() {
        historyModel.clear(NotificationManager.Notifications.ClearExpired);
    }

    /**
     * Toggles Do Not Disturb mode.
     */
    function toggleDoNotDisturbMode() {
        if (doNotDisturbModeEnabled) {
            notificationSettings.defaults();
        } else {
            var until = new Date();
            
            until.setFullYear(until.getFullYear() + 1);
            
            notificationSettings.notificationsInhibitedUntil = until;
        }
        
        notificationSettings.save();
    }
    
    /**
     * Open the system notification settings.
     */
    function openNotificationSettings() {
        MobileShell.ShellUtil.executeCommand("plasma-open-settings kcm_notifications");
    }

    PlasmaCore.DataSource {
        id: timeDataSource
        engine: "time"
        connectedSources: ["Local"]
        interval: 60000 // 1 min
        intervalAlignment: PlasmaCore.Types.AlignToMinute
    }
    
    // implement background clicking signal
    MouseArea {
        anchors.fill: parent
        onClicked: backgroundClicked()
        z: -1 // ensure that this is below notification items so we don't steal button clicks
    }

    ListView {
        id: list
        model: historyModel
        
        clip: true
        
        currentIndex: 0
        
        property var pendingNotificationWithAction

        readonly property bool listOverflowing: contentItem.childrenRect.height + toolButtons.height + spacing >= root.height
        
        height: count === 0 ? 0 : listOverflowing ? root.height - toolButtons.height : contentItem.childrenRect.height + spacing
        
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
                
        boundsBehavior: Flickable.StopAtBounds
        spacing: Kirigami.Units.largeSpacing

        // TODO keyboard focus
        highlightMoveDuration: 0
        highlightResizeDuration: 0
        highlight: Item {} 

        section {
            property: "isInGroup"
            criteria: ViewSection.FullString
        }
        
        PlasmaExtras.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (PlasmaCore.Units.largeSpacing * 4)

            text: i18n("Notification service not available")
            visible: list.count === 0 && !NotificationManager.Server.valid && historyModelType === NotificationsModelType.NotificationsModel

            PlasmaComponents3.Label {
                // Checking valid to avoid creating ServerInfo object if everything is alright
                readonly property NotificationManager.ServerInfo currentOwner: !NotificationManager.Server.valid ? NotificationManager.Server.currentOwner : null
                // PlasmaExtras.PlaceholderMessage is internally a ColumnLayout, so we can use Layout.whatever properties here
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: currentOwner ? i18nc("Vendor and product name", "Notifications are currently provided by '%1 %2'", currentOwner.vendor, currentOwner.name) : ""
                visible: currentOwner && currentOwner.vendor && currentOwner.name
            }
        }
        
        add: Transition {
            //SequentialAnimation {
                //PropertyAction { property: "opacity"; value: 0 }
                //PauseAnimation { duration: PlasmaCore.Units.longDuration }
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: PlasmaCore.Units.longDuration }
                    NumberAnimation { property: "height"; from: 0; duration: PlasmaCore.Units.longDuration }
                }
            //}
        }
        addDisplaced: Transition {
            NumberAnimation { properties: "y"; duration:  PlasmaCore.Units.longDuration }
        }
        remove: Transition {
            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation { property: "opacity"; from: 1; to: 0; duration: PlasmaCore.Units.longDuration }
                    NumberAnimation { property: "height"; to: 0; duration: PlasmaCore.Units.longDuration }
                }
                PauseAnimation { duration: PlasmaCore.Units.longDuration }
            }
        }
        removeDisplaced: Transition {
            SequentialAnimation {
                NumberAnimation { properties: "y"; duration:  PlasmaCore.Units.longDuration }
            }
        }
        moveDisplaced: Transition {
            SequentialAnimation {
                PauseAnimation { duration: PlasmaCore.Units.longDuration }
                NumberAnimation { properties: "y"; duration:  PlasmaCore.Units.longDuration }
            }
        }

        
        function isRowExpanded(row) {
            var idx = historyModel.index(row, 0);
            return historyModel.data(idx, NotificationManager.Notifications.IsGroupExpandedRole);
        }

        function setGroupExpanded(row, expanded) {
            var rowIdx = historyModel.index(row, 0);
            var persistentRowIdx = historyModel.makePersistentModelIndex(rowIdx);
            var persistentGroupIdx = historyModel.makePersistentModelIndex(historyModel.groupIndex(rowIdx));

            historyModel.setData(rowIdx, expanded, NotificationManager.Notifications.IsGroupExpandedRole);

            // If the current item went away when the group collapsed, scroll to the group heading
            if (!persistentRowIdx || !persistentRowIdx.valid) {
                if (persistentGroupIdx && persistentGroupIdx.valid) {
                    list.positionViewAtIndex(persistentGroupIdx.row, ListView.Contain);
                    // When closed via keyboard, also set a sane current index
                    if (list.currentIndex > -1) {
                        list.currentIndex = persistentGroupIdx.row;
                    }
                }
            }
            
            forceLayout();
        }
        
        delegate: Loader {
            id: delegateLoader
            
            anchors {
                left: parent ? parent.left : undefined
                leftMargin: PlasmaCore.Units.largeSpacing
                right: parent ? parent.right : undefined
                rightMargin: PlasmaCore.Units.largeSpacing
            }
            
            height: model.isGroup ? groupDelegate.height : notificationDelegate.height
            sourceComponent: model.isGroup ? groupDelegate : notificationDelegate
            
            required property var model
            required property int index
            
            Component {
                id: groupDelegate
                NotificationGroupHeader {
                    applicationName: model.applicationName
                    applicationIconSource: model.applicationIconName
                    originName: model.originName || ""
                    timeSource: timeDataSource
                }
            }
            
            Component {
                id: notificationDelegate
                
                Column {
                    spacing: PlasmaCore.Units.smallSpacing
                    
                    height: notificationItem.height + showMoreLoader.height
                    
                    NotificationItem {
                        id: notificationItem
                        width: parent.width
                        
                        model: delegateLoader.model
                        modelIndex: delegateLoader.index
                        notificationsModel: root.historyModel
                        notificationsModelType: root.historyModelType
                        timeSource: timeDataSource
                        
                        requestToInvoke: root.actionsRequireUnlock
                        onRunActionRequested: {
                            list.pendingNotificationWithAction = notificationItem;
                            root.unlockRequested();
                        }
                    }
                    
                    Loader {
                        id: showMoreLoader
                        
                        height: visible ? implicitHeight : 0
                        visible: active
                        active: {
                            // if we have the WatchedNotificationsModel, we don't have notification grouping support
                            if (typeof model.groupChildrenCount === 'undefined')
                                return false;
                            
                            return (model.groupChildrenCount > model.expandedGroupChildrenCount || model.isGroupExpanded)
                                && delegateLoader.ListView.nextSection != delegateLoader.ListView.section;
                        }
                        
                        sourceComponent: PlasmaComponents3.ToolButton {
                            icon.name: model.isGroupExpanded ? "arrow-up" : "arrow-down"
                            text: model.isGroupExpanded ? i18n("Show Fewer")
                                                        : i18nc("Expand to show n more notifications",
                                                                "Show %1 More", (model.groupChildrenCount - model.expandedGroupChildrenCount))
                            onClicked: {
                                list.setGroupExpanded(model.index, !model.isGroupExpanded)
                            }
                        }
                    }
                }
            }
        }
    }
    
    Item {
        id: toolButtons
            
        z: 2
        
        width: root.width
        height: toolLayout.implicitHeight + PlasmaCore.Units.largeSpacing
        
        anchors {
            top: list.bottom
            left: parent.left
            right: parent.right
        }
        
        Rectangle {
            visible: hasNotifications
                                
            anchors {
                top: toolButtons.top
                left: toolButtons.left
                right: toolButtons.right
            }
            
            height: 1
            
            opacity: 0.25
            color: PlasmaCore.Theme.textColor
        }
        
        RowLayout {
            id: toolLayout
            
            anchors {
                fill: parent
                leftMargin: PlasmaCore.Units.smallSpacing
                rightMargin: PlasmaCore.Units.smallSpacing
            }
            
            PlasmaComponents3.ToolButton {
                Layout.alignment: hasNotifications ? Qt.AlignLeft : Qt.AlignHCenter
                
                font.bold: true
                
                icon.name: doNotDisturbModeEnabled ? "notifications" : "notifications-disabled"
                text: doNotDisturbModeEnabled ? "Enable Notifications" : "Do Not Disturb"
                onClicked: {
                    toggleDoNotDisturbMode();
                }
            }
            
            PlasmaComponents3.ToolButton {
                id: clearButton
                
                Layout.alignment: Qt.AlignRight
                
                visible: hasNotifications
                
                font.bold: true
                
                icon.name: "edit-clear-history"
                text: "Clear All Notifications"
                onClicked: {
                    clearHistory();
                }
            }
        }
    }
}

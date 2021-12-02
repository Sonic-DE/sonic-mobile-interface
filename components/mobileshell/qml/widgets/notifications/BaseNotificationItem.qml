/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2018-2019 Kai Uwe Broulik <kde@privat.broulik.de>
 * 
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.8
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.notificationmanager 1.0 as NotificationManager

import org.kde.kcoreaddons 1.0 as KCoreAddons

Item {
    id: notificationItem
    required property NotificationManager.Notifications notificationsModel
    
    property var model
    property int modelIndex
    
    property PlasmaCore.DataSource timeSource
    
    property int notificationType: model.type

    property bool inGroup: model.isInGroup
    property bool inHistory: true

    property string applicationIconSource: model.applicationIconName
    property string applicationName: model.applicationName
    property string originName: model.originName || ""

    property string summary: model.summary
    property var time: model.updated || model.created

    property bool hasReplyAction: model.hasReplyAction || false
    property string replyActionLabel: model.replyActionLabel || ""
    property string replyPlaceholderText: model.replyPlaceholderText || ""
    property string replySubmitButtonText: model.replySubmitButtonText || ""
    property string replySubmitButtonIconName: model.replySubmitButtonIconName || ""
    
    // configure button on every single notifications is bit overwhelming
    property bool configurable: !inGroup && model.configurable

    property bool dismissable: model.type === NotificationManager.Notifications.JobType
        && model.jobState !== NotificationManager.Notifications.JobStateStopped
        && model.dismissed
        && notificationSettings.permanentJobPopups
    property bool dismissed: model.dismissed || false
    property bool closable: model.closable

    property string body: model.body || ""
    property var icon: model.image || model.iconName

    property var urls: model.urls || []

    property int jobState: model.jobState || 0
    property int percentage: model.percentage || 0
    property int jobError: model.jobError || 0
    property bool suspendable: !!model.suspendable
    property bool killable: !!model.killable
    
    property QtObject jobDetails: model.jobDetails || null

    property string configureActionLabel: model.configureActionLabel || ""
    readonly property bool addDefaultAction: (model.hasDefaultAction
                                            && model.defaultActionLabel
                                            && (model.actionLabels || []).indexOf(model.defaultActionLabel) === -1) ? true : false
    property var actionNames: {
        var actions = (model.actionNames || []);
        if (addDefaultAction) {
            actions.unshift("default"); // prepend
        }
        return actions;
    }
    property var actionLabels: {
        var labels = (model.actionLabels || []);
        if (addDefaultAction) {
            labels.unshift(model.defaultActionLabel);
        }
        return labels;
    }

    signal bodyClicked
    signal closeClicked
    signal configureClicked
    signal dismissClicked
    signal actionInvoked(string actionName)
    signal replied(string text)
    signal openUrl(string url)
    signal fileActionInvoked(QtObject action)

    signal suspendJobClicked
    signal resumeJobClicked
    signal killJobClicked
    
    onCloseClicked: close()
    onDismissClicked: model.dismissed = false;
    onConfigureClicked: notificationsModel.configure(notificationsModel.index(modelIndex, 0))

    onActionInvoked: {
        if (actionName === "default") {
            notificationsModel.invokeDefaultAction(notificationsModel.index(modelIndex, 0));
        } else {
            notificationsModel.invokeAction(notificationsModel.index(modelIndex, 0), actionName);
        }

        expire();
    }
    onOpenUrl: {
        Qt.openUrlExternally(url);
        expire();
    }
    onFileActionInvoked: {
        if (action.objectName === "movetotrash" || action.objectName === "deletefile") {
            close();
        } else {
            expire();
        }
    }
    onSuspendJobClicked: notificationsModel.suspendJob(notificationsModel.index(modelIndex, 0))
    onResumeJobClicked: notificationsModel.resumeJob(notificationsModel.index(modelIndex, 0))
    onKillJobClicked: notificationsModel.killJob(notificationsModel.index(modelIndex, 0))

    function expire() {
        if (model.resident) {
            model.expired = true;
        } else {
            notificationsModel.expire(notificationsModel.index(modelIndex, 0));
        }
    }

    function close() {
        notificationsModel.close(notificationsModel.index(modelIndex, 0));
    }
}


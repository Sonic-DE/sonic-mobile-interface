/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Layouts 1.4

import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.workspace.components 2.0 as PW
import org.kde.plasma.private.mobileshell as MobileShell

RowLayout {
    property real textPixelSize: Kirigami.Units.gridUnit * 0.6

    visible: MobileShell.BatteryInfo.isVisible


    ListView {
        id: batteryRepeater
        spacing: root.elementSpacing

        model: MobileShell.BatteryInfo.batteries

        orientation: ListView.Horizontal
        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: childrenRect.width
//        Layout.preferredWidth: height * 6
        Layout.fillHeight: true

        //spacing: Kirigami.Units.smallSpacing * 2

/*
        delegate: BatteryItem {
            width: scrollView.availableWidth

            batteryPercent: Percent
            batteryCapacity: Capacity
            batteryEnergy: Energy
            batteryPluggedIn: PluggedIn
            batteryIsPowerSupply: IsPowerSupply
            batteryChargeState: ChargeState
            batteryPrettyName: PrettyName
            batteryType: Type
            remainingTime: dialog.remainingTime

            KeyNavigation.up: index === 0 ? (batteryRepeater.headerItem.visible ? batteryRepeater.headerItem : batteryRepeater.headerItem.KeyNavigation.up) : batteryRepeater.itemAtIndex(index - 1)
            KeyNavigation.down: index + 1 < batteryRepeater.count ? batteryRepeater.itemAtIndex(index + 1) : batteryRepeater.footerItem

            pluggedIn: dialog.pluggedIn
            chargeStopThreshold: dialog.chargeStopThreshold

            KeyNavigation.backtab: KeyNavigation.up
            KeyNavigation.tab: KeyNavigation.down

            Keys.onTabPressed: event => {
                if (index === batteryRepeater.count - 1) {
                    // Workaround to leave applet's focus on desktop
                    nextItemInFocusChain(false).forceActiveFocus(Qt.TabFocusReason);
                } else {
                    event.accepted = false;
                }
            }

            onActiveFocusChanged: if (activeFocus) scrollView.positionViewAtItem(this)
        }
        */


        delegate: RowLayout {

            /* Battery properties (from batterycontrol.h):
             *     enum BatteryRoles {
                *  Percent = Qt::UserRole + 1,
                *  Capacity,
                *  Energy,
                *  PluggedIn,
                *  IsPowerSupply,
                *  ChargeState,
                *  PrettyName,
                *  Type }
                */

            Layout.preferredWidth: childrenRect.width
            Layout.fillHeight: true

            height: batteryLabel.height
            width: childrenRect.width + (root.elementSpacing * index)

            PW.BatteryIcon {
                id: battery
                //Layout.preferredWidth: height * 2
                Layout.fillHeight: true
                hasBattery: true
                // percent: MobileShell.BatteryInfo.percent
                // pluggedIn: MobileShell.BatteryInfo.pluggedIn
                percent: Percent
                pluggedIn: PluggedIn

                height: batteryLabel.height
                width: batteryLabel.height
            }

            PlasmaComponents.Label {
                id: batteryLabel
                //text: i18n("%1%", MobileShell.BatteryInfo.percent)
                text: i18n("%1%", Percent)
                //text: "hmm?"
                Layout.alignment: Qt.AlignVCenter

                color: Kirigami.Theme.textColor
                font.pixelSize: textPixelSize
            }
            // Rectangle {
            //
            //     anchors.fill: parent
            //     border.color: "red"
            // }
            Component.onCompleted: {
                console.log("======> Created Battery " + index);
                console.log("        PrettyName: " + PrettyName);
                console.log("        Percent:    " + Percent);
                console.log("        Type:       " + Type);
                console.log("        Energy:     " + Energy);
            }
        }
    }
}

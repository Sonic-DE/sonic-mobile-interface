// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.20 as Kirigami

import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

Kirigami.ApplicationWindow {
    id: root

    pageStack.initialPage: Kirigami.ScrollablePage {
        id: page

        title: i18n("Homescreen Settings")

        topPadding: 0
        bottomPadding: 0
        leftPadding: 0
        rightPadding: 0

        ColumnLayout {
            FormCard.FormHeader {
                title: i18n("General")
            }

            FormCard.FormCard {
                FormCard.FormButtonDelegate {
                    id: iconsButton
                    text: i18n('Icons')
                    icon.name: 'view-list-icons'
                    // onClicked: kcm.push("VibrationForm.qml")
                }
            }

            FormCard.FormHeader {
                title: i18n("Labels")
            }

            FormCard.FormCard {
                FormCard.FormCheckDelegate {
                    id: showLabelsOnHomeScreen
                    text: i18n("Show labels on homescreen")
                    checked: Folio.FolioSettings.showPagesAppLabels
                    onCheckedChanged: {
                        if (checked != Folio.FolioSettings.showPagesAppLabels) {
                            Folio.FolioSettings.showPagesAppLabels = checked;
                        }
                    }
                }

                FormCard.FormDelegateSeparator { above: showLabelsOnHomeScreen; below: showLabelsInFavourites }

                FormCard.FormCheckDelegate {
                    id: showLabelsInFavourites
                    text: i18n("Show labels in favorites bar")
                    checked: Folio.FolioSettings.showFavouritesAppLabels
                    onCheckedChanged: {
                        if (checked != Folio.FolioSettings.showFavouritesAppLabels) {
                            Folio.FolioSettings.showFavouritesAppLabels = checked;
                        }
                    }
                }
            }
        }
    }
}

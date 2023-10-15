// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include <Plasma/Applet>

class FolioSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int homeScreenRows READ homeScreenRows NOTIFY homeScreenRowsChanged)
    Q_PROPERTY(int homeScreenColumns READ homeScreenColumns NOTIFY homeScreenColumnsChanged)
    Q_PROPERTY(bool showFavouritesAppLabels READ showFavouritesAppLabels NOTIFY showFavouritesAppLabelsChanged)
    Q_PROPERTY(qreal homeScreenIconSize READ homeScreenIconSize NOTIFY homeScreenIconSizeChanged)

public:
    FolioSettings(QObject *parent = nullptr);

    static FolioSettings *self();

    // number of rows and columns in the config for the homescreen
    // NOTE: use HomeScreenState.pageRows() instead in UI logic since we may have the rows and
    //       columns swapped (in landscape layouts)
    int homeScreenRows();
    int homeScreenColumns();

    bool showFavouritesAppLabels();

    qreal homeScreenIconSize();

    Q_INVOKABLE void setApplet(Plasma::Applet *applet);

Q_SIGNALS:
    void homeScreenRowsChanged();
    void homeScreenColumnsChanged();
    void showFavouritesAppLabelsChanged();
    void homeScreenIconSizeChanged();

private:
    Plasma::Applet *m_applet{nullptr};
};

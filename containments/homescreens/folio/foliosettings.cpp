// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "foliosettings.h"

FolioSettings::FolioSettings(QObject *parent)
    : QObject{parent}
{
}

FolioSettings *FolioSettings::self()
{
    static FolioSettings *settings = new FolioSettings;
    return settings;
}

int FolioSettings::homeScreenRows()
{
    // TODO
    // ensure that this is fetched fast and cached (it is called extremely often)
    return 6;
}

int FolioSettings::homeScreenColumns()
{
    // TODO
    return 4;
}

bool FolioSettings::showFavouritesAppLabels()
{
    // TODO
    return false;
}

qreal FolioSettings::homeScreenIconSize()
{
    // TODO
    return 48;
}

void FolioSettings::setApplet(Plasma::Applet *applet)
{
    m_applet = applet;
}

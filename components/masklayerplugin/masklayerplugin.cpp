// SPDX-FileCopyrightText: 2025 Micah Stnaley <stanleymicah@proton.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "masklayerplugin.h"
#include "maskmanager.h"

void MaskLayerPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.private.mobile.homescreen.masklayerplugin"));

    qmlRegisterType<MaskManager>(uri, 1, 0, "MaskManager");
}

// SPDX-FileCopyrightText: 2023 by Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cellularplugin.h"

#include <QQmlContext>
#include <QQuickItem>

#include "cellularutil.h"

void CellularPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.plasma.mobileinitialstart.cellular"));

    qmlRegisterSingletonType<CellularUtil>(uri, 1, 0, "CellularUtil", [](QQmlEngine *, QJSEngine *) {
        return new CellularUtil;
    });
}

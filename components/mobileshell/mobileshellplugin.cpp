/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QQmlContext>
#include <QQuickItem>

#include "mobileshellplugin.h"
#include "shellutil.h"

void MobileShellPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.plasma.private.mobileshell"));

    qmlRegisterSingletonType<TimerPresetModel>(uri, 1, 0, "ShellUtil", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return ShellUtil::instance();
    });
}

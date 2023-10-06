// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "folioplugin.h"
#include "applicationlistmodel.h"
#include "favouritesmodel.h"
#include "folioapplication.h"
#include "folioapplicationfolder.h"
#include "foliodelegate.h"
#include "foliosettings.h"
#include "homescreenstate.h"
#include "pagelistmodel.h"
#include "pagemodel.h"

void HalcyonPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.private.mobile.homescreen.folio"));

    qmlRegisterSingletonType<ApplicationListModel>(uri, 1, 0, "ApplicationListModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return ApplicationListModel::self();
    });

    qmlRegisterSingletonType<FavouritesModel>(uri, 1, 0, "FavouritesModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return FavouritesModel::self();
    });

    qmlRegisterSingletonType<PageListModel>(uri, 1, 0, "PageListModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return PageListModel::self();
    });

    qmlRegisterSingletonType<FolioSettings>(uri, 1, 0, "FolioSettings", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return FolioSettings::self();
    });

    qmlRegisterType<HomeScreenState>(uri, 1, 0, "HomeScreenState");
    qmlRegisterType<FolioApplication>(uri, 1, 0, "FolioApplication");
    qmlRegisterType<FolioApplicationFolder>(uri, 1, 0, "FolioApplicationFolder");
    qmlRegisterType<FolioDelegate>(uri, 1, 0, "FolioDelegate");
    qmlRegisterType<PageModel>(uri, 1, 0, "PageModel");
    qmlRegisterType<FolioPageDelegate>(uri, 1, 0, "FolioPageDelegate");
}

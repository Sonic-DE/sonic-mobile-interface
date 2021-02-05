/*
 *   Copyright (C) 2014 Antonis Tsiapaliokas <antonis.tsiapaliokas@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Self
#include "favoritesmodel.h"

// Qt
#include <QByteArray>
#include <QModelIndex>
#include <QDebug>

// KDE
#include <KService>
#include <KSharedConfig>


constexpr int MAX_FAVOURITES = 5;

FavoritesModel::FavoritesModel(HomeScreen *parent)
    : ApplicationListModel(parent)
{
}

FavoritesModel::~FavoritesModel() = default;

void FavoritesModel::loadApplications()
{

    beginResetModel();

    m_applicationList.clear();

    QSet<QString> foundFavorites;
    QSet<QString> appsToRemove;

    for (const auto &appId : m_appOrder) {
        if (KService::Ptr service = KService::serviceByStorageId(appId)) {
            ApplicationData data;
            data.name = service->name();
            data.icon = service->icon();
            data.storageId = service->storageId();
            data.entryPath = service->exec();
            data.startupNotify = service->property(QStringLiteral("StartupNotify")).toBool();
            
            if (m_favorites.contains(data.storageId)) {
                data.location = Favorites;
                foundFavorites.insert(data.storageId);
            } else if (m_desktopItems.contains(data.storageId)) {
                data.location = Desktop;
            }

            m_applicationList << data;
        } else {
            appsToRemove.insert(appId);
        }
    }

    for (const auto &appId : appsToRemove) {
        m_appOrder.removeAll(appId);
    }
    
    endResetModel();
    emit countChanged();

    bool favChanged = false;
    for (const auto &item : m_favorites) {
        if (!foundFavorites.contains(item)) {
            favChanged = true;
            m_favorites.removeAll(item);
        }
    }
    if (favChanged) {
        if (m_applet) {
            m_applet->config().writeEntry("Favorites", m_favorites);
        }
        emit favoriteCountChanged();
    }
}

void FavoritesModel::addFavorite(const QString &storageId, int row, LauncherLocation location)
{
    if (row < 0 || row > m_applicationList.count()) {
        return;
    }

    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        ApplicationData data;
        data.name = service->name();
        data.icon = service->icon();
        data.storageId = service->storageId();
        data.entryPath = service->exec();
        data.startupNotify = service->property(QStringLiteral("StartupNotify")).toBool();

        bool favChanged = false;
        if (location == Favorites) {
            data.location = Favorites;
            m_favorites.insert(qMin(row, m_favorites.count()), storageId);
            favChanged = true;
        } else {
            data.location = location;
        }

        beginInsertRows(QModelIndex(), row, row);
        m_applicationList.insert(row, data);
        m_appOrder.insert(row, storageId);
        endInsertRows();
        if (favChanged) {
            emit favoriteCountChanged();
        }
    }
}

void FavoritesModel::removeFavorite(int row)
{
    if (row < 0 || row >= m_applicationList.count()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    const QString storageId = m_applicationList[row].storageId;
    m_appOrder.removeAll(storageId);
    const bool favChanged = m_favorites.contains(storageId);
    m_favorites.removeAll(storageId);
    m_appPositions.remove(storageId);
    m_applicationList.removeAt(row);
    endRemoveRows();
    if (favChanged) {
        emit favoriteCountChanged();
    }
}

void FavoritesModel::removeMatchingFavorites(const QString &storageId)
{
    QMutableListIterator<ApplicationListModel::ApplicationData> i(m_applicationList);
    int row = 0;

    while (i.hasNext()) {
        i.next();
        const QString sid = i.value().storageId;
        
        if (sid == storageId) {
            beginRemoveRows(QModelIndex(), row, row);
            m_appOrder.removeAll(sid);
            const bool favChanged = m_favorites.contains(sid);
            m_favorites.removeAll(sid);
            m_appPositions.remove(sid);
            i.remove();
            endRemoveRows();
            if (favChanged) {
                emit favoriteCountChanged();
            }
        } else {
            ++row;
        }
    }
}

#include "moc_favoritesmodel.cpp"


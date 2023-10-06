// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "favouritesmodel.h"

#include <QByteArray>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QModelIndex>
#include <QProcess>
#include <QQuickWindow>

#include <KApplicationTrader>
#include <KConfigGroup>
#include <KIO/ApplicationLauncherJob>
#include <KNotificationJobUiDelegate>
#include <KService>
#include <KSharedConfig>
#include <KSycoca>

FavouritesModel *FavouritesModel::self()
{
    static FavouritesModel *inst = new FavouritesModel();
    return inst;
}

FavouritesModel::FavouritesModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

int FavouritesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_delegates.count();
}

QVariant FavouritesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case DelegateRole:
        return QVariant::fromValue(m_delegates.at(index.row()));
    }

    return QVariant();
}

QHash<int, QByteArray> FavouritesModel::roleNames() const
{
    return {{DelegateRole, "delegate"}};
}

void FavouritesModel::addApp(const QString &storageId, int row)
{
    if (row < 0 || row > m_delegates.size()) {
        return;
    }

    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        FolioApplication *app = new FolioApplication{this, service};
        FolioDelegate *delegate = new FolioDelegate{app, this};

        beginInsertRows(QModelIndex(), row, row);
        m_delegates.insert(row, delegate);
        endInsertRows();

        save();
    }
}

void FavouritesModel::removeEntry(int row)
{
    if (row < 0 || row >= m_delegates.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    if (m_delegates[row]) {
        m_delegates[row]->deleteLater();
    }
    m_delegates.removeAt(row);
    endRemoveRows();

    save();
}

void FavouritesModel::moveEntry(int fromRow, int toRow)
{
    if (fromRow < 0 || toRow < 0 || fromRow >= m_delegates.length() || toRow >= m_delegates.length() || fromRow == toRow) {
        return;
    }
    if (toRow > fromRow) {
        ++toRow;
    }

    beginMoveRows(QModelIndex(), fromRow, fromRow, QModelIndex(), toRow);
    if (toRow > fromRow) {
        FolioDelegate *delegate = m_delegates.at(fromRow);
        m_delegates.insert(toRow, delegate);
        m_delegates.takeAt(fromRow);

    } else {
        FolioDelegate *delegate = m_delegates.takeAt(fromRow);
        m_delegates.insert(toRow, delegate);
    }
    endMoveRows();

    save();
}

void FavouritesModel::save()
{
    if (!m_applet) {
        return;
    }

    QJsonArray arr;
    for (int i = 0; i < m_delegates.size(); i++) {
        // if this delegate is empty, ignore it
        if (!m_delegates[i]) {
            continue;
        }

        arr.append(m_delegates[i]->toJson());
    }
    QByteArray data = QJsonDocument(arr).toJson(QJsonDocument::Compact);

    m_applet->config().writeEntry("Favourites", QString::fromStdString(data.toStdString()));
    Q_EMIT m_applet->configNeedsSaving();
}

void FavouritesModel::load()
{
    if (!m_applet) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(m_applet->config().readEntry("Favourites", "{}").toUtf8());

    beginResetModel();

    for (QJsonValueRef r : doc.array()) {
        QJsonObject obj = r.toObject();

        FolioDelegate *delegate = FolioDelegate::fromJson(obj, this);

        if (delegate) {
            if (delegate->type() == FolioDelegate::Folder) {
                connect(delegate->folder(), &FolioApplicationFolder::saveRequested, this, &FavouritesModel::save);
            }

            m_delegates.append(delegate);
        }
    }

    endResetModel();
}

void FavouritesModel::setApplet(Plasma::Applet *applet)
{
    m_applet = applet;
    load();
}

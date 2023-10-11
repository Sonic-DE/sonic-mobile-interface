// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "favouritesmodel.h"
#include "homescreenstate.h"

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
    connect(HomeScreenState::self(), &HomeScreenState::pageWidthChanged, this, &FavouritesModel::evaluateDelegatePositions);
    connect(HomeScreenState::self(), &HomeScreenState::pageCellWidthChanged, this, &FavouritesModel::evaluateDelegatePositions);
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
        return QVariant::fromValue(m_delegates.at(index.row()).delegate);
    case XPositionRole:
        return QVariant::fromValue(m_delegates.at(index.row()).xPosition);
    }

    return QVariant();
}

QHash<int, QByteArray> FavouritesModel::roleNames() const
{
    return {{DelegateRole, "delegate"}, {XPositionRole, "xPosition"}};
}

void FavouritesModel::removeEntry(int row)
{
    if (row < 0 || row >= m_delegates.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    if (m_delegates[row].delegate) {
        m_delegates[row].delegate->deleteLater();
    }
    m_delegates.removeAt(row);
    endRemoveRows();

    evaluateDelegatePositions();

    save();
}

void FavouritesModel::moveEntry(int fromRow, int toRow)
{
    if (fromRow < 0 || toRow < 0 || fromRow >= m_delegates.size() || toRow >= m_delegates.size() || fromRow == toRow) {
        return;
    }
    if (toRow > fromRow) {
        ++toRow;
    }

    beginMoveRows(QModelIndex(), fromRow, fromRow, QModelIndex(), toRow);
    if (toRow > fromRow) {
        auto delegate = m_delegates.at(fromRow);
        m_delegates.insert(toRow, delegate);
        m_delegates.takeAt(fromRow);

    } else {
        auto delegate = m_delegates.takeAt(fromRow);
        m_delegates.insert(toRow, delegate);
    }
    endMoveRows();

    evaluateDelegatePositions();

    save();
}

bool FavouritesModel::addEntry(int row, FolioDelegate *delegate)
{
    if (!delegate) {
        return false;
    }

    if (row < 0 || row > m_delegates.size()) {
        return false;
    }

    if (row == m_delegates.size()) {
        beginInsertRows(QModelIndex(), row, row);
        m_delegates.append({delegate, 0});
        endInsertRows();
    } else if (m_delegates[row].delegate->type() == FolioDelegate::None) {
        replaceGhostEntry(delegate);
    } else {
        beginInsertRows(QModelIndex(), row, row);
        m_delegates.insert(row, {delegate, 0});
        endInsertRows();
    }

    evaluateDelegatePositions();

    save();

    return true;
}

FolioDelegate *FavouritesModel::getEntryAt(int row)
{
    if (row < 0 || row >= m_delegates.size()) {
        return nullptr;
    }

    return m_delegates[row].delegate;
}

void FavouritesModel::setGhostEntry(int row)
{
    bool found = false;
    for (int i = 0; i < m_delegates.size(); i++) {
        if (m_delegates[i].delegate->type() == FolioDelegate::None) {
            found = true;
            moveEntry(i, row);
        }
    }

    if (!found) {
        FolioDelegate *ghost = new FolioDelegate{this};
        addEntry(row, ghost);
    }
}

void FavouritesModel::replaceGhostEntry(FolioDelegate *delegate)
{
    for (int i = 0; i < m_delegates.size(); i++) {
        if (m_delegates[i].delegate->type() == FolioDelegate::None) {
            m_delegates[i].delegate = delegate;

            Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, 0), {DelegateRole});
            break;
        }
    }
}

void FavouritesModel::deleteGhostEntry()
{
    for (int i = 0; i < m_delegates.size(); i++) {
        if (m_delegates[i].delegate->type() == FolioDelegate::None) {
            removeEntry(i);
        }
    }
}

void FavouritesModel::save()
{
    if (!m_applet) {
        return;
    }

    QJsonArray arr;
    for (int i = 0; i < m_delegates.size(); i++) {
        // if this delegate is empty, ignore it
        if (!m_delegates[i].delegate || m_delegates[i].delegate->type() == FolioDelegate::None) {
            continue;
        }

        arr.append(m_delegates[i].delegate->toJson());
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

            m_delegates.append({delegate, 0});
        }
    }

    evaluateDelegatePositions();

    endResetModel();
}

bool FavouritesModel::dropPositionIsEdge(qreal x)
{
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();
    qreal startPosition = getDelegateRowStartX();

    if (x < startPosition) {
        return true;
    }

    int index = x / cellWidth;
    qreal delegateCentre = index * cellWidth + cellWidth / 2;

    // if it is within the centre 70% of a delegate, it is not at an edge
    return qAbs(delegateCentre - x) >= cellWidth * 0.35; // 0.35 since we are measuring from centre
}

int FavouritesModel::dropInsertPosition(qreal x)
{
    qreal startPosition = getDelegateRowStartX();
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();

    int pos = (x - startPosition) / cellWidth;
    return std::min((int)m_delegates.size(), std::max(0, pos));
}

void FavouritesModel::setApplet(Plasma::Applet *applet)
{
    m_applet = applet;
    load();
}

void FavouritesModel::evaluateDelegatePositions()
{
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();
    qreal startPosition = getDelegateRowStartX();

    for (int i = 0; i < m_delegates.size(); ++i) {
        m_delegates[i].xPosition = qRound(startPosition + cellWidth * i);
    }

    Q_EMIT dataChanged(createIndex(0, 0), createIndex(m_delegates.size() - 1, 0), {XPositionRole});
}

qreal FavouritesModel::getDelegateRowStartX()
{
    int length = m_delegates.size();
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();
    qreal pageWidth = HomeScreenState::self()->pageWidth();

    return (pageWidth / 2) - (((qreal)length) / 2) * cellWidth;
}

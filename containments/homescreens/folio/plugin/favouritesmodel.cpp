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
    connect(HomeScreenState::self(), &HomeScreenState::pageWidthChanged, this, [this]() {
        evaluateDelegatePositions(true);
    });
    connect(HomeScreenState::self(), &HomeScreenState::pageCellWidthChanged, this, [this]() {
        evaluateDelegatePositions(true);
    });
}

int FavouritesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_delegates.count();
}

QVariant FavouritesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_delegates.size()) {
        return QVariant();
    }

    switch (role) {
    case DelegateRole:
        return QVariant::fromValue(m_delegates.at(index.row()).delegate);
    case XPositionRole:
        return QVariant::fromValue(m_delegates.at(index.row()).xPosition);
    case HiddenRole:
        return m_delegates.at(index.row()).delegate == m_invisibleDelegate;
    }

    return QVariant();
}

QHash<int, QByteArray> FavouritesModel::roleNames() const
{
    return {{DelegateRole, "delegate"}, {XPositionRole, "xPosition"}, {HiddenRole, "hidden"}};
}

void FavouritesModel::removeEntry(int row)
{
    if (row < 0 || row >= m_delegates.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    // HACK: do not deleteLater(), because the delegate might still be used somewhere else
    // m_delegates[row].delegate->deleteLater();
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
        evaluateDelegatePositions(false);
        endInsertRows();
    } else if (m_delegates[row].delegate->type() == FolioDelegate::None) {
        replaceGhostEntry(delegate);
    } else {
        beginInsertRows(QModelIndex(), row, row);
        m_delegates.insert(row, {delegate, 0});
        evaluateDelegatePositions(false);
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

int FavouritesModel::getGhostEntryPosition()
{
    for (int i = 0; i < m_delegates.size(); i++) {
        if (m_delegates[i].delegate->type() == FolioDelegate::None) {
            return i;
        }
    }
    return -1;
}

void FavouritesModel::setGhostEntry(int row)
{
    bool found = false;

    // check if a ghost entry already exists, then swap them
    for (int i = 0; i < m_delegates.size(); i++) {
        if (m_delegates[i].delegate->type() == FolioDelegate::None) {
            found = true;

            if (row != i) {
                moveEntry(i, row);
            }
        }
    }

    // if it doesn't, add a new empty delegate
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

void FavouritesModel::setInvisiblePosition(int row)
{
    if (row < 0 || row >= m_delegates.size()) {
        return;
    }

    m_invisibleDelegate = m_delegates[row].delegate;
    evaluateDelegatePositions();
}

void FavouritesModel::clearInvisiblePosition()
{
    m_invisibleDelegate = nullptr;
    evaluateDelegatePositions();
}

void FavouritesModel::save()
{
    if (!m_applet) {
        return;
    }

    QJsonArray arr;
    for (int i = 0; i < m_delegates.size(); i++) {
        FolioDelegate *delegate = m_delegates[i].delegate;

        // if this delegate is empty, ignore it
        if (!delegate || delegate->type() == FolioDelegate::None) {
            continue;
        }

        arr.append(delegate->toJson());
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

    qreal currentX = startPosition;

    for (int i = 0; i < m_delegates.size(); i++) {
        // ignore invisible delegate
        if (m_delegates[i].delegate == m_invisibleDelegate) {
            continue;
        }

        qDebug() << "drop position compare" << x << currentX << cellWidth;

        // if it is within the centre 70% of a delegate, it is not at an edge
        if (x >= currentX + cellWidth * 0.15 && x <= currentX + cellWidth * 0.85) {
            return false;
        }

        currentX += cellWidth;
    }

    qDebug() << "false";

    return true;
}

int FavouritesModel::dropInsertPosition(qreal x)
{
    qreal startPosition = getDelegateRowStartX();
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();

    if (x < startPosition) {
        return 0;
    }

    qreal currentX = startPosition;
    for (int i = 0; i < m_delegates.size(); i++) {
        // ignore invisible delegate
        if (m_delegates[i].delegate == m_invisibleDelegate) {
            continue;
        }

        if (x < currentX + cellWidth * 0.85) {
            return i;
        } else if (x < currentX + cellWidth) {
            return i + 1;
        }

        currentX += cellWidth;
    }
    return m_delegates.size();
}

void FavouritesModel::setApplet(Plasma::Applet *applet)
{
    m_applet = applet;
    load();
}

void FavouritesModel::evaluateDelegatePositions(bool emitSignal)
{
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();
    qreal startPosition = getDelegateRowStartX();

    qreal currentX = startPosition;

    for (int i = 0; i < m_delegates.size(); ++i) {
        m_delegates[i].xPosition = qRound(currentX);

        if (m_delegates[i].delegate != m_invisibleDelegate) {
            currentX += cellWidth;
        }
    }

    if (emitSignal) {
        Q_EMIT dataChanged(createIndex(0, 0), createIndex(m_delegates.size() - 1, 0), {HiddenRole});
        Q_EMIT dataChanged(createIndex(0, 0), createIndex(m_delegates.size() - 1, 0), {XPositionRole});
    }
}

qreal FavouritesModel::getDelegateRowStartX()
{
    int length = m_delegates.size();
    qreal cellWidth = HomeScreenState::self()->pageCellWidth();
    qreal pageWidth = HomeScreenState::self()->pageWidth();

    if (m_invisibleDelegate != nullptr) {
        length--;
    }

    return (pageWidth / 2) - (((qreal)length) / 2) * cellWidth;
}

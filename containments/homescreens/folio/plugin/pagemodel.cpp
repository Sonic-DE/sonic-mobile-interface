// SPDX-FileCopyrightText: 2022-2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pagemodel.h"
#include "foliosettings.h"

FolioPageDelegate::FolioPageDelegate(int row, int column, QObject *parent)
    : FolioDelegate{parent}
    , m_row{row}
    , m_column{column}
{
}

FolioPageDelegate::FolioPageDelegate(int row, int column, FolioApplication *application, QObject *parent)
    : FolioDelegate{application, parent}
    , m_row{row}
    , m_column{column}
{
}

FolioPageDelegate::FolioPageDelegate(int row, int column, FolioApplicationFolder *folder, QObject *parent)
    : FolioDelegate{folder, parent}
    , m_row{row}
    , m_column{column}
{
}

FolioPageDelegate *FolioPageDelegate::fromJson(QJsonObject &obj, QObject *parent)
{
    FolioPageDelegate *delegate = nullptr;

    int row = obj[QStringLiteral("row")].toInt();
    int column = obj[QStringLiteral("column")].toInt();

    // TODO remove duplication from FolioDelegate with some shared function
    QString type = obj[QStringLiteral("type")].toString();
    if (type == "application") {
        // read application
        FolioApplication *app = FolioApplication::fromJson(obj, parent);

        if (app) {
            delegate = new FolioPageDelegate{row, column, app, parent};
        }

    } else if (type == "folder") {
        // read folder
        FolioApplicationFolder *folder = FolioApplicationFolder::fromJson(obj, parent);

        if (folder) {
            delegate = new FolioPageDelegate{row, column, folder, parent};
        }

    } else if (type == "none") {
        delegate = new FolioPageDelegate{row, column, parent};
    }

    return delegate;
}

QJsonObject FolioPageDelegate::toJson() const
{
    QJsonObject o = FolioDelegate::toJson();
    o[QStringLiteral("row")] = m_row;
    o[QStringLiteral("column")] = m_column;
    return o;
}

int FolioPageDelegate::row()
{
    return m_row;
}

void FolioPageDelegate::setRow(int row)
{
    m_row = row;
    Q_EMIT rowChanged();
}

int FolioPageDelegate::column()
{
    return m_column;
}

void FolioPageDelegate::setColumn(int column)
{
    m_column = column;
    Q_EMIT columnChanged();
}

PageModel::PageModel(QList<FolioPageDelegate *> delegates, QObject *parent)
    : QAbstractListModel{parent}
    , m_delegates{delegates}
{
}

PageModel::~PageModel() = default;

PageModel *PageModel::fromJson(QJsonArray &arr, QObject *parent)
{
    QList<FolioPageDelegate *> delegates;
    QList<FolioPageDelegate *> folderDelegates;

    for (QJsonValueRef r : arr) {
        QJsonObject obj = r.toObject();

        FolioPageDelegate *delegate = FolioPageDelegate::fromJson(obj, parent);
        if (delegate) {
            // TODO check if row and column value makes sense
            delegates.append(delegate);

            if (delegate->type() == FolioDelegate::Folder) {
                folderDelegates.append(delegate);
            }
        }
    }

    PageModel *model = new PageModel{delegates, parent};

    // ensure folders request saves
    for (auto *delegate : folderDelegates) {
        connect(delegate->folder(), &FolioApplicationFolder::saveRequested, model, &PageModel::save);
    }

    return model;
}

QJsonArray PageModel::toJson() const
{
    QJsonArray arr;

    for (FolioPageDelegate *delegate : m_delegates) {
        if (!delegate) {
            continue;
        }

        arr.append(delegate->toJson());
    }

    return arr;
}

int PageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_delegates.size();
}

QVariant PageModel::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> PageModel::roleNames() const
{
    return {{DelegateRole, "delegate"}};
}

void PageModel::addAppDelegate(int row, int col, QString storageId)
{
    if (row < 0 || row > FolioSettings::self()->homeScreenRows() || col < 0 || col > FolioSettings::self()->homeScreenColumns()) {
        return;
    }

    // check if there already exists a delegate in this space
    for (FolioPageDelegate *delegate : m_delegates) {
        if (row == delegate->row() && col == delegate->column()) {
            return;
        }
    }

    // insert if the app is valid
    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        FolioApplication *app = new FolioApplication{this, service};
        FolioPageDelegate *delegate = new FolioPageDelegate{row, col, app, this};

        beginInsertRows(QModelIndex(), row, row);
        m_delegates.append(delegate);
        endInsertRows();

        save();
    }
}

void PageModel::removeDelegate(int row, int col)
{
    bool removed = false;

    for (int i = 0; i < m_delegates.size(); ++i) {
        if (m_delegates[i]->row() == row && m_delegates[i]->column() == col) {
            beginRemoveRows(QModelIndex(), i, i);
            m_delegates.removeAt(i);
            endRemoveRows();

            removed = true;
        }
    }

    if (removed) {
        save();
    }
}

void PageModel::save()
{
    Q_EMIT saveRequested();
}

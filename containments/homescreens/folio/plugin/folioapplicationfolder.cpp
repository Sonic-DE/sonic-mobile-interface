// SPDX-FileCopyrightText: 2022-2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "folioapplicationfolder.h"

#include <QJsonArray>

FolioApplicationFolder::FolioApplicationFolder(QObject *parent, QString name)
    : QObject{parent}
    , m_name{name}
    , m_applicationFolderModel{new ApplicationFolderModel{this}}
{
}

FolioApplicationFolder *FolioApplicationFolder::fromJson(QJsonObject &obj, QObject *parent)
{
    QString name = obj[QStringLiteral("name")].toString();
    QList<FolioApplication *> apps;
    for (auto storageId : obj[QStringLiteral("apps")].toArray()) {
        if (KService::Ptr service = KService::serviceByStorageId(storageId.toString())) {
            apps.append(new FolioApplication(parent, service));
        }
    }

    FolioApplicationFolder *folder = new FolioApplicationFolder(parent, name);
    folder->setApplications(apps);
    return folder;
}

QJsonObject FolioApplicationFolder::toJson()
{
    QJsonObject obj;
    obj[QStringLiteral("type")] = "folder";
    obj[QStringLiteral("name")] = m_name;

    QJsonArray arr;
    for (auto *application : m_applications) {
        arr.append(QJsonValue::fromVariant(application->storageId()));
    }

    obj[QStringLiteral("apps")] = arr;

    return obj;
}

QString FolioApplicationFolder::name() const
{
    return m_name;
}

void FolioApplicationFolder::setName(QString &name)
{
    m_name = name;
    Q_EMIT nameChanged();
    Q_EMIT saveRequested();
}

QList<FolioApplication *> FolioApplicationFolder::appPreviews()
{
    QList<FolioApplication *> previews;
    // we give a maximum of 4 icons
    for (int i = 0; i < std::min<int>(m_applications.length(), 4); ++i) {
        previews.push_back(m_applications[i]);
    }
    return previews;
}

ApplicationFolderModel *FolioApplicationFolder::applications()
{
    return m_applicationFolderModel;
}

void FolioApplicationFolder::setApplications(QList<FolioApplication *> applications)
{
    if (m_applicationFolderModel) {
        m_applicationFolderModel->deleteLater();
    }

    m_applications = applications;
    m_applicationFolderModel = new ApplicationFolderModel{this};

    Q_EMIT applicationsChanged();
    Q_EMIT applicationsReset();
    Q_EMIT saveRequested();
}

void FolioApplicationFolder::moveEntry(int fromRow, int toRow)
{
    m_applicationFolderModel->moveEntry(fromRow, toRow);
}

void FolioApplicationFolder::addApp(const QString &storageId, int row)
{
    m_applicationFolderModel->addApp(storageId, row);
}

void FolioApplicationFolder::removeApp(int row)
{
    m_applicationFolderModel->removeApp(row);
}

void FolioApplicationFolder::moveAppOut(int row)
{
    if (row < 0 || row >= m_applications.size()) {
        return;
    }

    Q_EMIT moveAppOutRequested(m_applications[row]->storageId());
    removeApp(row);
}

ApplicationFolderModel::ApplicationFolderModel(FolioApplicationFolder *folder)
    : QAbstractListModel{folder}
    , m_folder{folder}
{
}

int ApplicationFolderModel::rowCount(const QModelIndex &parent) const
{
    return m_folder->m_applications.size();
}

QVariant ApplicationFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case ApplicationRole:
        return QVariant::fromValue(m_folder->m_applications.at(index.row()));
    }

    return QVariant();
}

QHash<int, QByteArray> ApplicationFolderModel::roleNames() const
{
    return {{ApplicationRole, "application"}};
}

void ApplicationFolderModel::moveEntry(int fromRow, int toRow)
{
    if (fromRow < 0 || toRow < 0 || fromRow >= m_folder->m_applications.length() || toRow >= m_folder->m_applications.length() || fromRow == toRow) {
        return;
    }
    if (toRow > fromRow) {
        ++toRow;
    }

    beginMoveRows(QModelIndex(), fromRow, fromRow, QModelIndex(), toRow);
    if (toRow > fromRow) {
        FolioApplication *app = m_folder->m_applications.at(fromRow);
        m_folder->m_applications.insert(toRow, app);
        m_folder->m_applications.takeAt(fromRow);
    } else {
        FolioApplication *app = m_folder->m_applications.takeAt(fromRow);
        m_folder->m_applications.insert(toRow, app);
    }
    endMoveRows();
    Q_EMIT m_folder->applicationsChanged();
    Q_EMIT m_folder->saveRequested();
}

void ApplicationFolderModel::addApp(const QString &storageId, int row)
{
    if (row < 0 || row > m_folder->m_applications.size()) {
        return;
    }

    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        beginInsertRows(QModelIndex(), row, row);
        FolioApplication *app = new FolioApplication(this, service);
        m_folder->m_applications.insert(row, app);
        endInsertRows();

        Q_EMIT m_folder->applicationsChanged();
        Q_EMIT m_folder->saveRequested();
    }
}

void ApplicationFolderModel::removeApp(int row)
{
    if (row < 0 || row >= m_folder->m_applications.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_folder->m_applications[row]->deleteLater();
    m_folder->m_applications.removeAt(row);
    endRemoveRows();

    Q_EMIT m_folder->applicationsChanged();
    Q_EMIT m_folder->saveRequested();
}

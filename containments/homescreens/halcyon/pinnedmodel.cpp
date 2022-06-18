// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pinnedmodel.h"

#include <QJsonArray>
#include <QJsonDocument>

PinnedModel::PinnedModel(QObject *parent, Plasma::Applet *applet)
    : QAbstractListModel{parent}
    , m_applet{applet}
{
}

PinnedModel::~PinnedModel() = default;

int PinnedModel::rowCount(const QModelIndex &parent) const
{
    return m_applications.count();
}

QVariant PinnedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case IsFolderRole:
        return m_folders.at(index.row()) != nullptr;
    case ApplicationRole:
        return QVariant::fromValue(m_applications.at(index.row()));
    case FolderRole:
        return QVariant::fromValue(m_folders.at(index.row()));
    }

    return QVariant();
}

QHash<int, QByteArray> PinnedModel::roleNames() const
{
    return {{IsFolderRole, "isFolder"}, {ApplicationRole, "application"}, {FolderRole, "folder"}};
}

void PinnedModel::addFavorite(const QString &storageId, int row)
{
    // TODO
    save();
}

void PinnedModel::removeFavorite(int row)
{
    // TODO
    save();
}

void PinnedModel::load()
{
    if (!m_applet) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(m_applet->config().readEntry("Pinned", "{}").toUtf8());

    beginResetModel();

    for (QJsonValueRef r : doc.array()) {
        QJsonObject obj = r.toObject();

        if (obj[QStringLiteral("type")].toString() == "application") {
            // read application
            Application *app = Application::fromJson(obj, this);
            if (app) {
                m_applications.append(app);
                m_folders.append(nullptr);
            }

        } else if (obj[QStringLiteral("type")].toString() == "folder") {
            // read folder
            ApplicationFolder *folder = ApplicationFolder::fromJson(obj, this);
            if (folder) {
                m_applications.append(nullptr);
                m_folders.append(folder);
            }
        }
    }

    endResetModel();
}

void PinnedModel::save()
{
    if (!m_applet) {
        return;
    }

    // TODO
}

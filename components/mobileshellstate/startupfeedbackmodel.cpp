// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "startupfeedback.h"
#include "windowlistener.h"

StartupFeedback::StartupFeedback(QObject *parent, QString appId, QString iconName, qreal iconStartX, qreal iconStartY, qreal iconSize, int screen)
    : QObject{parent}
    , m_appId{appId}
    , m_iconName{iconName}
    , m_iconStartX{iconStartX}
    , m_iconStartY{iconStartY}
    , m_iconSize{iconSize}
    , m_screen{screen}
{
}

QString StartupFeedback::appId()
{
    return m_appId;
}

QString StartupFeedback::iconName()
{
    return m_iconName;
}

qreal StartupFeedback::iconStartX()
{
    return m_iconStartX;
}

qreal StartupFeedback::iconStartY()
{
    return m_iconStartY;
}

qreal StartupFeedback::iconSize()
{
    return m_iconSize;
}

int StartupFeedback::screen()
{
    return m_screen;
}

StartupFeedbackModel::StartupFeedbackModel(QObject *parent)
    : QAbstractListModel{parent}
{
    connect(WindowListener::instance(), &WindowListener::windowCreated, this, [this](QString storageId) {
        QList<int> indicesToRemove;

        // Remove startupfeedback when the respective window is created
        for (auto *startupFeedback : m_list) {
            if (startupFeedback->appId() == storageId) {
                indicesToRemove.remove(storageId);
            }
        }

        for (int index : indicesToRemove) {
            Q_EMIT beginRemoveRows(QModelIndex{}, index, index);
            m_list[index]->deleteLater();
            m_list.removeAt(index);
            Q_EMIT endRemoveRows();
        }
    });
}

void StartupFeedbackModel::addApp(StartupFeedback *startupFeedback)
{
    Q_EMIT begininsertRows(QModelIndex{}, m_list.size(), m_list.size());
    m_list.append(startupFeedback);
    Q_EMIT endInsertRows();
}

int StartupFeedbackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.count();
}

QVariant StartupFeedbackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case DelegateRole:
        return QVariant::fromValue(m_list[index]);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> StartupFeedbackModel::roleNames() const
{
    return {{DelegateRole, QByteArrayLiteral("delegate")}};
}
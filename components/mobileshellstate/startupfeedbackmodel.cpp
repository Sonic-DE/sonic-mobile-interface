// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "startupfeedbackmodel.h"

constexpr int STARTUP_FEEDBACK_TIMEOUT_MS = 8000;

StartupFeedback::StartupFeedback(QObject *parent,
                                 QString iconName,
                                 QString title,
                                 QString storageId,
                                 qreal iconStartX,
                                 qreal iconStartY,
                                 qreal iconSize,
                                 int screen)
    : QObject{parent}
    , m_iconName{iconName}
    , m_title{title}
    , m_storageId{storageId}
    , m_iconStartX{iconStartX}
    , m_iconStartY{iconStartY}
    , m_iconSize{iconSize}
    , m_screen{screen}
    , m_timeoutTimer{new QTimer{this}}
{
    connect(m_timeoutTimer, &QTimer::timeout, this, &StartupFeedback::timeout);
}

QString StartupFeedback::iconName() const
{
    return m_iconName;
}

QString StartupFeedback::title() const
{
    return m_title;
}

QString StartupFeedback::storageId() const
{
    return m_storageId;
}

qreal StartupFeedback::iconStartX() const
{
    return m_iconStartX;
}

qreal StartupFeedback::iconStartY() const
{
    return m_iconStartY;
}

qreal StartupFeedback::iconSize() const
{
    return m_iconSize;
}

int StartupFeedback::screen() const
{
    return m_screen;
}

QString StartupFeedback::windowUuid() const
{
    return m_windowUuid;
}

void StartupFeedback::setWindowUuid(QString uuid)
{
    m_windowUuid = uuid;
}

void StartupFeedback::startTimeoutTimer()
{
    // Timeout of 5 seconds before closing
    m_timeoutTimer->start(STARTUP_FEEDBACK_TIMEOUT_MS);
}

StartupFeedbackModel::StartupFeedbackModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

void StartupFeedbackModel::addApp(StartupFeedback *startupFeedback)
{
    beginInsertRows(QModelIndex{}, m_list.size(), m_list.size());

    m_list.append(startupFeedback);
    updateActiveWindowIsStartupFeedback();

    startupFeedback->startTimeoutTimer();

    connect(startupFeedback, &StartupFeedback::timeout, this, [this, startupFeedback]() {
        int index = m_list.indexOf(startupFeedback);
        if (index == -1) {
            return;
        }

        beginRemoveRows(QModelIndex{}, index, index);
        m_list.removeAt(index);
        updateActiveWindowIsStartupFeedback();
        endRemoveRows();
    });

    // Prepare state for active window being startupfeedback early, otherwise we have a race condition between
    // the Plasma window opening and the visual (causes panels to flash background color)
    m_activeWindowIsStartupFeedback = true;
    Q_EMIT activeWindowIsStartupFeedbackChanged();

    endInsertRows();
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

    auto delegate = m_list[index.row()];

    switch (role) {
    case DelegateRole:
        return QVariant::fromValue(delegate);
    case ScreenRole:
        return delegate->screen();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> StartupFeedbackModel::roleNames() const
{
    return {{DelegateRole, QByteArrayLiteral("delegate")}, {ScreenRole, QByteArrayLiteral("screen")}};
}

bool StartupFeedbackModel::activeWindowIsStartupFeedback() const
{
    return m_activeWindowIsStartupFeedback;
}

void StartupFeedbackModel::updateActiveWindowIsStartupFeedback()
{
    bool isStartupFeedback = false;

    if (isStartupFeedback != m_activeWindowIsStartupFeedback) {
        m_activeWindowIsStartupFeedback = isStartupFeedback;
        Q_EMIT activeWindowIsStartupFeedbackChanged();
    }
}

StartupFeedbackFilterModel::StartupFeedbackFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterRole(StartupFeedbackModel::ScreenRole);
}

StartupFeedbackModel *StartupFeedbackFilterModel::startupFeedbackModel() const
{
    return m_startupFeedbackModel;
}

void StartupFeedbackFilterModel::setStartupFeedbackModel(StartupFeedbackModel *startupFeedbackModel)
{
    if (startupFeedbackModel == m_startupFeedbackModel) {
        return;
    }

    m_startupFeedbackModel = startupFeedbackModel;
    setSourceModel(m_startupFeedbackModel);
    Q_EMIT startupFeedbackModelChanged();
}

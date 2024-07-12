// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <qqmlregistration.h>

// TODO: add kwin script to merge startup feedback with window

class StartupFeedback : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString storageId READ storageId CONSTANT)
    Q_PROPERTY(qreal iconStartX READ iconStartX CONSTANT)
    Q_PROPERTY(qreal iconStartY READ iconStartY CONSTANT)
    Q_PROPERTY(qreal iconSize READ iconSize CONSTANT)
    Q_PROPERTY(int screen READ screen CONSTANT)

public:
    explicit StartupFeedback(QObject *parent = nullptr,
                             QString iconName = "",
                             QString title = "",
                             QString storageId = "",
                             qreal iconStartX = 0.0,
                             qreal iconStartY = 0.0,
                             qreal iconSize = 0.0,
                             int screen = 0);

    explicit StartupFeedback();

    QString iconName();
    QString title();
    QString storageId();

    qreal iconStartX();
    qreal iconStartY();
    qreal iconSize();

    int screen();

    void startTimeoutTimer();

Q_SIGNALS:
    void timeout();

private:
    QString m_iconName;
    QString m_title;
    QString m_storageId;
    qreal m_iconStartX{0.0};
    qreal m_iconStartY{0.0};
    qreal m_iconSize{0.0};
    int m_screen{0};

    QTimer *m_timeoutTimer{nullptr};
};

class StartupFeedbackModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        DelegateRole = Qt::UserRole,
        ScreenRole,
    };

    explicit StartupFeedbackModel(QObject *parent = nullptr);

    void addApp(StartupFeedback *startupFeedback);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private Q_SLOTS:
    void onWindowOpened(QString storageId);

private:
    void init();

    QList<StartupFeedback *> m_list;
};

class StartupFeedbackFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(StartupFeedbackModel *startupFeedbackModel READ startupFeedbackModel WRITE setStartupFeedbackModel NOTIFY startupFeedbackModelChanged)
    Q_PROPERTY(int screen READ screen WRITE setScreen NOTIFY screenChanged)

public:
    explicit StartupFeedbackFilterModel(QObject *parent = nullptr);

    StartupFeedbackModel *startupFeedbackModel() const;
    void setStartupFeedbackModel(StartupFeedbackModel *taskModel);

    int screen() const;
    void setScreen(int screen);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

Q_SIGNALS:
    void screenChanged();
    void startupFeedbackModelChanged();

private:
    StartupFeedbackModel *m_startupFeedbackModel{nullptr};
    int m_screen{0};
};

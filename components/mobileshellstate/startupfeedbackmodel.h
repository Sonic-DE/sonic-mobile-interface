// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

// TODO: add kwin script to merge startup feedback with window

class StartupFeedback : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString appId READ appId CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(qreal iconStartX READ iconStartX CONSTANT)
    Q_PROPERTY(qreal iconStartY READ iconStartY CONSTANT)
    Q_PROPERTY(qreal iconSize READ iconSize CONSTANT)
    Q_PROPERTY(int screen READ screen CONSTANT)

public:
    explicit StartupFeedback(QObject *parent = nullptr,
                             QString appId = "",
                             QString iconName = "",
                             qreal iconStartX = 0.0,
                             qreal iconStartY = 0.0,
                             qreal iconSize = 0.0,
                             int screen = 0);

    QString appId();
    QString iconName();

    qreal iconStartX();
    qreal iconStartY();
    qreal iconSize();

    int screen();

private:
    QString m_appId;
    QString m_iconName;
    qreal m_iconStartX{0.0};
    qreal m_iconStartY{0.0};
    qreal m_iconSize{0.0};
    int m_screen{0};
};

class StartupFeedbackModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        DelegateRole = Qt::UserRole,
    };

    explicit StartupFeedbackModel(QObject *parent = nullptr);

    void addApp(StartupFeedback *startupFeedback);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private Q_SLOTS:
    void onWindowCreated();

private:
    void init();

    QList<StartupFeedback *> m_list;
};

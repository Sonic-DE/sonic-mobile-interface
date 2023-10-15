// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "pagemodel.h"

#include <QAbstractListModel>
#include <QList>

#include <Plasma/Applet>

class PageListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles { PageRole = Qt::UserRole + 1 };

    PageListModel(QObject *parent = nullptr);

    static PageListModel *self();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    PageModel *getPage(int index);
    void removePage(int index);
    Q_INVOKABLE void addPageAtEnd();
    bool isLastPageEmpty();

    void save();

private:
    void load();

    QList<PageModel *> m_pages;
};

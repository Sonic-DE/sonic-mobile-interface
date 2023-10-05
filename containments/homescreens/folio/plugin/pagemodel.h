// SPDX-FileCopyrightText: 2022-2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "folioapplication.h"
#include "folioapplicationfolder.h"

#include <QAbstractListModel>
#include <QList>

#include <Plasma/Applet>

class PageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { IsFolderRole = Qt::UserRole + 1, ApplicationRole, FolderRole };

    PageModel(int page = 0, QObject *parent = nullptr);
    ~PageModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void save();

    Plasma::Applet *applet();
    void setApplet(Plasma::Applet *applet);

Q_SIGNALS:
    void appletChanged();

private:
    void load();

    QList<QList<FolioDelegate *>> m_applications;

    Plasma::Applet *m_applet;
};

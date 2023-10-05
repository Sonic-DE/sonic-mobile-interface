// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QQuickItem>
#include <QSet>

#include <Plasma/Applet>

#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/plasmawindowmanagement.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>

#include "foliodelegate.h"

class FavouritesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Plasma::Applet *applet READ applet WRITE setApplet NOTIFY appletChanged)

public:
    enum Roles {
        DelegateRole = Qt::UserRole + 1,
    };

    FavouritesModel(QObject *parent = nullptr);
    static FavouritesModel *self();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addApp(const QString &storageId, int row);
    Q_INVOKABLE void removeEntry(int row);
    Q_INVOKABLE void moveEntry(int fromRow, int toRow);
    void save();

    Plasma::Applet *applet();
    void setApplet(Plasma::Applet *applet);

Q_SIGNALS:
    void appletChanged();

private:
    void load();

    int m_columns;
    QList<FolioDelegate *> m_delegates;

    Plasma::Applet *m_applet;
};

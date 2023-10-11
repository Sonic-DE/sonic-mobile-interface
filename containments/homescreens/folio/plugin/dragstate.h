// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

#include "foliodelegate.h"
#include "homescreenstate.h"

class HomeScreenState;

class DelegateDragPosition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DelegateDragPosition::Location location READ location NOTIFY locationChanged)
    Q_PROPERTY(int page READ page NOTIFY pageChanged)
    Q_PROPERTY(int pageRow READ pageRow NOTIFY pageRowChanged)
    Q_PROPERTY(int pageColumn READ pageColumn NOTIFY pageColumnChanged)
    Q_PROPERTY(int favouritesPosition READ favouritesPosition NOTIFY favouritesPositionChanged)

public:
    enum Location { Pages, Favourites, AppDrawer };
    Q_ENUM(Location)

    DelegateDragPosition(QObject *parent = nullptr);
    ~DelegateDragPosition();

    void copyFrom(DelegateDragPosition *position);

    Location location() const;
    void setLocation(Location location);

    int page() const;
    void setPage(int page);

    int pageRow() const;
    void setPageRow(int pageRow);

    int pageColumn() const;
    void setPageColumn(int pageColumn);

    int favouritesPosition() const;
    void setFavouritesPosition(int favouritesPosition);

Q_SIGNALS:
    void locationChanged();
    void pageChanged();
    void pageRowChanged();
    void pageColumnChanged();
    void favouritesPositionChanged();

private:
    Location m_location{DelegateDragPosition::Pages};
    int m_page;
    int m_pageRow;
    int m_pageColumn;
    int m_favouritesPosition;
};

Q_DECLARE_METATYPE(DelegateDragPosition);

class DragState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DelegateDragPosition *candidateDropPosition READ candidateDropPosition CONSTANT)
    Q_PROPERTY(DelegateDragPosition *startPosition READ startPosition CONSTANT)

public:
    DragState(HomeScreenState *state = nullptr, QObject *parent = nullptr);

    DelegateDragPosition *candidateDropPosition() const;
    DelegateDragPosition *startPosition() const;

private Q_SLOTS:
    void onDelegateDragPositionChanged();
    void onDelegateDragFromPageStarted(int page, int row, int column);
    void onDelegateDragFromFavouritesStarted(int position);
    void onDelegateDragFromAppDrawerStarted(QString storageId);
    void onDelegateDropped();
    void onChangePageTimerFinished();
    void onFavouritesInsertBetweenTimerFinished();

private:
    void deleteStartPositionDelegate();
    void createDropPositionDelegate(bool modifyFolders);
    bool isStartPositionEqualDropPosition();

    // we need to adjust so that the coord is in the center of the delegate
    qreal getDraggedDelegateX();
    qreal getDraggedDelegateY();

    QTimer *m_changePageTimer;
    QTimer *m_favouritesInsertBetweenTimer; // inserting between apps
    int m_favouritesInsertBetweenIndex;

    HomeScreenState *m_state;

    FolioDelegate *m_dropDelegate;

    // where we are hovering over, potentially to drop the delegate
    DelegateDragPosition *const m_candidateDropPosition;
    // this is the original start position of the drag
    DelegateDragPosition *const m_startPosition;
};

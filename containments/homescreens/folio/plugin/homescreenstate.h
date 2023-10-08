// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "qqml.h"
#include <QObject>
#include <QPropertyAnimation>

#include "dragstate.h"

class DragState;

/**
 * @short The homescreen state, containing information on positioning panels as well as any swipe events.
 *
 * @author Devin Lin <devin@kde.org>
 */

class HomeScreenState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(HomeScreenState::SwipeState swipeState READ swipeState NOTIFY swipeStateChanged)
    Q_PROPERTY(HomeScreenState::ViewState viewState READ viewState NOTIFY viewStateChanged)
    Q_PROPERTY(DragState *dragState READ dragState CONSTANT)

    Q_PROPERTY(qreal pageViewX READ pageViewX WRITE setPageViewX NOTIFY pageViewXChanged)
    Q_PROPERTY(qreal pageWidth READ pageWidth WRITE setPageWidth NOTIFY pageWidthChanged)
    Q_PROPERTY(qreal pageHeight READ pageHeight WRITE setPageHeight NOTIFY pageHeightChanged)
    Q_PROPERTY(qreal pageContentWidth READ pageContentWidth WRITE setPageContentWidth NOTIFY pageContentWidthChanged)
    Q_PROPERTY(qreal pageContentHeight READ pageContentHeight WRITE setPageContentHeight NOTIFY pageContentHeightChanged)
    Q_PROPERTY(qreal pageCellWidth READ pageCellWidth WRITE setPageCellWidth NOTIFY pageCellWidthChanged)
    Q_PROPERTY(qreal pageCellHeight READ pageCellHeight WRITE setPageCellHeight NOTIFY pageCellHeightChanged)

    Q_PROPERTY(qreal appDrawerOpenProgress READ appDrawerOpenProgress NOTIFY appDrawerOpenProgressChanged)
    Q_PROPERTY(qreal appDrawerY READ appDrawerY WRITE setAppDrawerY NOTIFY appDrawerYChanged)

    Q_PROPERTY(qreal searchWidgetOpenProgress READ searchWidgetOpenProgress NOTIFY searchWidgetOpenProgressChanged)
    Q_PROPERTY(qreal searchWidgetY READ searchWidgetY WRITE setSearchWidgetY NOTIFY searchWidgetYChanged)

    Q_PROPERTY(qreal delegateDragX READ delegateDragX NOTIFY delegateDragXChanged)
    Q_PROPERTY(qreal delegateDragY READ delegateDragY NOTIFY delegateDragYChanged)

    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)
    Q_CLASSINFO("DefaultProperty", "children")
    QML_NAMED_ELEMENT("HomeScreenState")

public:
    enum SwipeState {
        None,
        DeterminingSwipeType,
        SwipingPages,
        SwipingOpenAppDrawer,
        SwipingCloseAppDrawer,
        SwipingOpenSearchWidget,
        SwipingCloseSearchWidget,
        AwaitingDraggingDelegate,
        DraggingDelegate,
    };
    Q_ENUM(SwipeState)

    enum ViewState {
        SearchWidgetView,
        PageView,
        AppDrawerView,
    };
    Q_ENUM(ViewState)

    HomeScreenState(QObject *parent = nullptr);

    // the current state of swipe interaction
    SwipeState swipeState();

    // the current view
    ViewState viewState();

    DragState *dragState();

    // the current horizontal position of the pageview
    // starts at 0, each page is m_pageWidth wide
    // first page is at -m_pageWidth, second is at -m_pageWidth * 2, etc.
    qreal pageViewX();
    void setPageViewX(qreal pageViewX);

    // the width of a single pageview page (set from QML)
    qreal pageWidth() const;
    void setPageWidth(qreal pageWidth);

    qreal pageHeight() const;
    void setPageHeight(qreal pageHeight);

    qreal pageContentWidth() const;
    void setPageContentWidth(qreal pageContentWidth);

    qreal pageContentHeight() const;
    void setPageContentHeight(qreal pageContentHeight);

    qreal pageCellWidth() const;
    void setPageCellWidth(qreal pageCellWidth);

    qreal pageCellHeight() const;
    void setPageCellHeight(qreal pageCellHeight);

    // between 0-1, the progress for the opening of the app drawer
    qreal appDrawerOpenProgress();

    // the position of the app drawer
    // 0: the app drawer is open
    // APP_DRAWER_OPEN_DIST: - the app drawer is closed
    qreal appDrawerY();
    void setAppDrawerY(qreal appDrawerY);

    // between 0-1, the progress for the opening of the search widget
    qreal searchWidgetOpenProgress();

    // the position of the search widget
    // 0: the search widget
    // SEARCH_WIDGET_OPEN_DIST: - the app drawer is closed
    qreal searchWidgetY();
    void setSearchWidgetY(qreal searchWidgetY);

    qreal delegateDragX();
    void setDelegateDragX(qreal delegateDragX);

    qreal delegateDragY();
    void setDelegateDragY(qreal delegateDragY);

    int currentPage();

    QQmlListProperty<QObject> children();

Q_SIGNALS:
    void swipeStateChanged();
    void viewStateChanged();
    void pageViewXChanged();
    void pageWidthChanged();
    void pageHeightChanged();
    void pageContentWidthChanged();
    void pageContentHeightChanged();
    void pageCellWidthChanged();
    void pageCellHeightChanged();
    void appDrawerOpenProgressChanged();
    void appDrawerYChanged();
    void searchWidgetOpenProgressChanged();
    void searchWidgetYChanged();
    void delegateDragXChanged();
    void delegateDragYChanged();
    void delegateDragEnded();
    void delegateDragFromPageStarted(int page, int row, int column);
    void delegateDragFromFavouritesStarted(int position);
    void delegateDragFromAppDrawerStarted(QString storageId);
    void pageNumChanged();

public Q_SLOTS:
    void openAppDrawer();
    void closeAppDrawer();
    void openSearchWidget();
    void closeSearchWidget();
    void snapPage(); // snaps to closest page
    void goToPage(int page);
    void startDelegatePageDrag(qreal startX, qreal startY, int page, int row, int column);
    void startDelegateFavouritesDrag(qreal startX, qreal startY, int position);
    void startDelegateAppDrawerDrag(qreal startX, qreal startY, QString storageId);

    // from SwipeArea
    void swipeStarted();
    void swipeEnded();
    void swipeMoved(qreal totalDeltaX, qreal totalDeltaY, qreal deltaX, qreal deltaY);

private:
    void setViewState(ViewState viewState);
    void setSwipeState(SwipeState swipeState);

    void startDelegateDrag(qreal startX, qreal startY);

    void cancelAppDrawerAnimations();
    void cancelSearchWidgetAnimations();

    // check if we passed the swipe threshold, and determine the swipe type after
    void determineSwipeTypeAfterThreshold(qreal totalDeltaX, qreal totalDeltaY);

    SwipeState m_swipeState{SwipeState::None};
    ViewState m_viewState{ViewState::PageView};

    DragState *m_dragState;

    qreal m_pageViewX{0};
    qreal m_pageWidth{0};
    qreal m_pageHeight{0};
    qreal m_pageContentWidth{0};
    qreal m_pageContentHeight{0};
    qreal m_pageCellWidth{0};
    qreal m_pageCellHeight{0};

    qreal m_appDrawerOpenProgress{0};
    qreal m_appDrawerY{0};
    qreal m_searchWidgetOpenProgress{0};
    qreal m_searchWidgetY{0};
    qreal m_delegateDragX{0};
    qreal m_delegateDragY{0};

    int m_pageNum{0};

    bool m_movingUp{false};
    bool m_movingRight{false};

    QPropertyAnimation *m_openAppDrawerAnim{nullptr};
    QPropertyAnimation *m_closeAppDrawerAnim{nullptr};
    QPropertyAnimation *m_openSearchWidgetAnim{nullptr};
    QPropertyAnimation *m_closeSearchWidgetAnim{nullptr};
    QPropertyAnimation *m_pageAnim{nullptr};

    QList<QObject *> m_children;
};

QML_DECLARE_TYPE(HomeScreenState)

// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "qqml.h"
#include <QObject>
#include <QPropertyAnimation>

#include <Plasma/Applet>

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

    Q_PROPERTY(qreal viewWidth READ viewWidth WRITE setViewWidth NOTIFY viewWidthChanged)
    Q_PROPERTY(qreal viewHeight READ viewHeight WRITE setViewHeight NOTIFY viewHeightChanged)

    Q_PROPERTY(bool columnRowSwap READ columnRowSwap NOTIFY columnRowSwapChanged)
    Q_PROPERTY(int pageRows READ pageRows NOTIFY pageRowsChanged)
    Q_PROPERTY(int pageColumns READ pageColumns NOTIFY pageColumnsChanged)

    Q_PROPERTY(qreal pageViewX READ pageViewX WRITE setPageViewX NOTIFY pageViewXChanged)
    Q_PROPERTY(qreal pageWidth READ pageWidth WRITE setPageWidth NOTIFY pageWidthChanged)
    Q_PROPERTY(qreal pageHeight READ pageHeight WRITE setPageHeight NOTIFY pageHeightChanged)
    Q_PROPERTY(qreal pageContentWidth READ pageContentWidth WRITE setPageContentWidth NOTIFY pageContentWidthChanged)
    Q_PROPERTY(qreal pageContentHeight READ pageContentHeight WRITE setPageContentHeight NOTIFY pageContentHeightChanged)
    Q_PROPERTY(qreal pageCellWidth READ pageCellWidth WRITE setPageCellWidth NOTIFY pageCellWidthChanged)
    Q_PROPERTY(qreal pageCellHeight READ pageCellHeight WRITE setPageCellHeight NOTIFY pageCellHeightChanged)

    Q_PROPERTY(qreal folderViewX READ folderViewX WRITE setFolderViewX NOTIFY folderViewXChanged)
    Q_PROPERTY(qreal folderPageWidth READ folderPageWidth WRITE setFolderPageWidth NOTIFY folderPageWidthChanged)
    Q_PROPERTY(qreal folderPageHeight READ folderPageHeight WRITE setFolderPageHeight NOTIFY folderPageHeightChanged)
    Q_PROPERTY(qreal folderPageContentWidth READ folderPageContentWidth WRITE setFolderPageContentWidth NOTIFY folderPageContentWidthChanged)
    Q_PROPERTY(qreal folderPageContentHeight READ folderPageContentHeight WRITE setFolderPageContentHeight NOTIFY folderPageContentHeightChanged)
    Q_PROPERTY(qreal folderOpenProgress READ folderOpenProgress WRITE setFolderOpenProgress NOTIFY folderOpenProgressChanged)
    Q_PROPERTY(FolioApplicationFolder *currentFolder READ currentFolder NOTIFY currentFolderChanged)

    Q_PROPERTY(qreal appDrawerOpenProgress READ appDrawerOpenProgress NOTIFY appDrawerOpenProgressChanged)
    Q_PROPERTY(qreal appDrawerY READ appDrawerY WRITE setAppDrawerY NOTIFY appDrawerYChanged)

    Q_PROPERTY(qreal searchWidgetOpenProgress READ searchWidgetOpenProgress NOTIFY searchWidgetOpenProgressChanged)
    Q_PROPERTY(qreal searchWidgetY READ searchWidgetY WRITE setSearchWidgetY NOTIFY searchWidgetYChanged)

    Q_PROPERTY(qreal delegateDragX READ delegateDragX NOTIFY delegateDragXChanged)
    Q_PROPERTY(qreal delegateDragY READ delegateDragY NOTIFY delegateDragYChanged)

public:
    enum SwipeState {
        None,
        DeterminingSwipeType,
        SwipingPages,
        SwipingOpenAppDrawer,
        SwipingCloseAppDrawer,
        SwipingOpenSearchWidget,
        SwipingCloseSearchWidget,
        SwipingFolderPages,
        AwaitingDraggingDelegate,
        DraggingDelegate,
    };
    Q_ENUM(SwipeState)

    enum ViewState {
        SearchWidgetView,
        PageView,
        AppDrawerView,
        FolderView,
    };
    Q_ENUM(ViewState)

    static HomeScreenState *self();

    HomeScreenState(QObject *parent = nullptr);

    // the current state of swipe interaction
    SwipeState swipeState() const;

    // the current view
    ViewState viewState() const;

    // drag state object
    DragState *dragState() const;

    qreal viewWidth() const;
    void setViewWidth(qreal viewWidth);

    qreal viewHeight() const;
    void setViewHeight(qreal viewHeight);

    // whether to swap rows and columns in the layout
    // this happens if the width of the screen is larger than the height
    bool columnRowSwap() const;
    void setColumnRowSwap(bool columnRowSwap);

    // the number of rows on a page
    int pageRows() const;
    void setPageRows(int pageRows);

    // the number of columns on a page
    int pageColumns() const;
    void setPageColumns(int pageColumns);

    // the current horizontal position of the pageview
    // starts at 0, each page is m_pageWidth wide
    // first page is at -m_pageWidth, second is at -m_pageWidth * 2, etc.
    qreal pageViewX() const;
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

    qreal folderViewX() const;
    void setFolderViewX(qreal folderViewX);

    qreal folderPageWidth() const;
    void setFolderPageWidth(qreal folderPageWidth);

    qreal folderPageHeight() const;
    void setFolderPageHeight(qreal folderPageHeight);

    qreal folderPageContentWidth() const;
    void setFolderPageContentWidth(qreal folderPageContentWidth);

    qreal folderPageContentHeight() const;
    void setFolderPageContentHeight(qreal folderPageContentHeight);

    qreal folderOpenProgress() const;
    void setFolderOpenProgress(qreal folderOpenProgress);

    FolioApplicationFolder *currentFolder() const;
    void setCurrentFolder(FolioApplicationFolder *folder);

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
    int currentFolderPage();

Q_SIGNALS:
    void swipeStateChanged();
    void viewStateChanged();
    void viewWidthChanged();
    void viewHeightChanged();
    void columnRowSwapChanged();
    void pageRowsChanged();
    void pageColumnsChanged();
    void pageViewXChanged();
    void pageWidthChanged();
    void pageHeightChanged();
    void pageContentWidthChanged();
    void pageContentHeightChanged();
    void pageCellWidthChanged();
    void pageCellHeightChanged();
    void folderViewXChanged();
    void folderPageWidthChanged();
    void folderPageHeightChanged();
    void folderPageContentWidthChanged();
    void folderPageContentHeightChanged();
    void folderOpenProgressChanged();
    void currentFolderChanged();
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
    void delegateDragFromFolderStarted(FolioApplicationFolder *folder, int position);
    void pageNumChanged();
    void folderPageNumChanged();

    void leftCurrentFolder();

public Q_SLOTS:
    void openAppDrawer();
    void closeAppDrawer();
    void openSearchWidget();
    void closeSearchWidget();

    void snapPage(); // snaps to closest page
    void goToPage(int page);

    void goToFolderPage(int page);
    void openFolder(FolioApplicationFolder *folder);
    void closeFolder();

    void startDelegatePageDrag(qreal startX, qreal startY, int page, int row, int column);
    void startDelegateFavouritesDrag(qreal startX, qreal startY, int position);
    void startDelegateAppDrawerDrag(qreal startX, qreal startY, QString storageId);
    void startDelegateFolderDrag(qreal startX, qreal startY, FolioApplicationFolder *folder, int position);
    void cancelDelegateDrag();

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

    DragState *m_dragState{nullptr};

    qreal m_viewWidth{0};
    qreal m_viewHeight{0};

    bool m_columnRowSwap{false};
    int m_pageRows{0};
    int m_pageColumns{0};

    qreal m_pageViewX{0};
    qreal m_pageWidth{0};
    qreal m_pageHeight{0};
    qreal m_pageContentWidth{0};
    qreal m_pageContentHeight{0};
    qreal m_pageCellWidth{0};
    qreal m_pageCellHeight{0};

    qreal m_folderViewX{0};
    qreal m_folderPageWidth{0};
    qreal m_folderPageHeight{0};
    qreal m_folderPageContentWidth{0};
    qreal m_folderPageContentHeight{0};
    qreal m_folderOpenProgress{0};
    FolioApplicationFolder *m_currentFolder{nullptr};

    qreal m_appDrawerOpenProgress{0};
    qreal m_appDrawerY{0};
    qreal m_searchWidgetOpenProgress{0};
    qreal m_searchWidgetY{0};
    qreal m_delegateDragX{0};
    qreal m_delegateDragY{0};

    int m_pageNum{0};
    int m_folderPageNum{0};

    bool m_movingUp{false};
    bool m_movingRight{false};

    QPropertyAnimation *m_openAppDrawerAnim{nullptr};
    QPropertyAnimation *m_closeAppDrawerAnim{nullptr};
    QPropertyAnimation *m_openSearchWidgetAnim{nullptr};
    QPropertyAnimation *m_closeSearchWidgetAnim{nullptr};
    QPropertyAnimation *m_pageAnim{nullptr};
    QPropertyAnimation *m_openFolderAnim{nullptr};
    QPropertyAnimation *m_closeFolderAnim{nullptr};
    QPropertyAnimation *m_folderPageAnim{nullptr};
};

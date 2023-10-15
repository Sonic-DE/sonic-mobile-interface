// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "homescreenstate.h"
#include "foliosettings.h"
#include "pagelistmodel.h"

#include <algorithm>

// TODO don't hardcode, use something more device dependent?
const qreal APP_DRAWER_OPEN_DIST = 300;
const qreal SEARCH_WIDGET_OPEN_DIST = 300;

// pixels to move before we determine the swipe type
const qreal DETERMINE_SWIPE_THRESHOLD = 10;

HomeScreenState *HomeScreenState::self()
{
    static HomeScreenState *inst = new HomeScreenState{nullptr};
    return inst;
}

HomeScreenState::HomeScreenState(QObject *parent)
    : QObject{parent}
    , m_dragState{new DragState{this, this}}
    , m_appDrawerY{APP_DRAWER_OPEN_DIST}
    , m_searchWidgetY{SEARCH_WIDGET_OPEN_DIST}
{
    m_openAppDrawerAnim = new QPropertyAnimation{this, "appDrawerY", this};
    m_openAppDrawerAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_openAppDrawerAnim->setEndValue(0);
    m_openAppDrawerAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_openAppDrawerAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::AppDrawerView);
    });

    m_closeAppDrawerAnim = new QPropertyAnimation{this, "appDrawerY", this};
    m_closeAppDrawerAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_closeAppDrawerAnim->setEndValue(APP_DRAWER_OPEN_DIST);
    m_closeAppDrawerAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_closeAppDrawerAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::PageView);
    });

    m_openSearchWidgetAnim = new QPropertyAnimation{this, "searchWidgetY", this};
    m_openSearchWidgetAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_openSearchWidgetAnim->setEndValue(0);
    m_openSearchWidgetAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_openSearchWidgetAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::SearchWidgetView);
    });

    m_closeSearchWidgetAnim = new QPropertyAnimation{this, "searchWidgetY", this};
    m_closeSearchWidgetAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_closeSearchWidgetAnim->setEndValue(SEARCH_WIDGET_OPEN_DIST);
    m_closeSearchWidgetAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_closeSearchWidgetAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::PageView);
    });

    m_pageAnim = new QPropertyAnimation{this, "pageViewX", this};
    m_pageAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_pageAnim->setEasingCurve(QEasingCurve::OutCubic);

    m_openFolderAnim = new QPropertyAnimation{this, "folderOpenProgress", this};
    m_openFolderAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_openFolderAnim->setEndValue(1.0);
    m_openFolderAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_openFolderAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::FolderView);
    });

    m_closeFolderAnim = new QPropertyAnimation{this, "folderOpenProgress", this};
    m_closeFolderAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_closeFolderAnim->setEndValue(0.0);
    m_closeFolderAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_closeFolderAnim, &QPropertyAnimation::finished, this, [this]() {
        setViewState(ViewState::PageView);
        setCurrentFolder(nullptr);
        setFolderViewX(0); // reset to first page

        Q_EMIT leftCurrentFolder();
    });

    m_folderPageAnim = new QPropertyAnimation{this, "folderViewX", this};
    m_folderPageAnim->setDuration(200 * 2); // TODO use kirigami longDuration * 2
    m_folderPageAnim->setEasingCurve(QEasingCurve::OutCubic);

    connect(this, &HomeScreenState::viewWidthChanged, this, [this]() {
        setColumnRowSwap(m_viewWidth > m_viewHeight);
    });
    connect(this, &HomeScreenState::viewHeightChanged, this, [this]() {
        setColumnRowSwap(m_viewWidth > m_viewHeight);
    });
    connect(this, &HomeScreenState::columnRowSwapChanged, this, [this]() {
        setPageRows(m_columnRowSwap ? FolioSettings::self()->homeScreenColumns() : FolioSettings::self()->homeScreenRows());
        setPageColumns(m_columnRowSwap ? FolioSettings::self()->homeScreenRows() : FolioSettings::self()->homeScreenColumns());
    });
}

HomeScreenState::ViewState HomeScreenState::viewState() const
{
    return m_viewState;
}

void HomeScreenState::setViewState(ViewState viewState)
{
    if (viewState != m_viewState) {
        m_viewState = viewState;
        Q_EMIT viewStateChanged();
    }
}

HomeScreenState::SwipeState HomeScreenState::swipeState() const
{
    return m_swipeState;
}

void HomeScreenState::setSwipeState(SwipeState swipeState)
{
    if (swipeState != m_swipeState) {
        m_swipeState = swipeState;
        Q_EMIT swipeStateChanged();
    }
}

DragState *HomeScreenState::dragState() const
{
    return m_dragState;
}

qreal HomeScreenState::viewWidth() const
{
    return m_viewWidth;
}

void HomeScreenState::setViewWidth(qreal viewWidth)
{
    if (m_viewWidth != viewWidth) {
        m_viewWidth = viewWidth;
        Q_EMIT viewWidthChanged();
    }
}

qreal HomeScreenState::viewHeight() const
{
    return m_viewHeight;
}

void HomeScreenState::setViewHeight(qreal viewHeight)
{
    if (m_viewHeight != viewHeight) {
        m_viewHeight = viewHeight;
        Q_EMIT viewHeightChanged();
    }
}

bool HomeScreenState::columnRowSwap() const
{
    return m_viewWidth > m_viewHeight;
}

void HomeScreenState::setColumnRowSwap(bool columnRowSwap)
{
    if (m_columnRowSwap != columnRowSwap) {
        m_columnRowSwap = columnRowSwap;
        Q_EMIT columnRowSwapChanged();
    }
}

int HomeScreenState::pageRows() const
{
    return columnRowSwap() ? FolioSettings::self()->homeScreenColumns() : FolioSettings::self()->homeScreenRows();
}

void HomeScreenState::setPageRows(int pageRows)
{
    if (m_pageRows != pageRows) {
        m_pageRows = pageRows;
        Q_EMIT pageRowsChanged();
    }
}

int HomeScreenState::pageColumns() const
{
    return columnRowSwap() ? FolioSettings::self()->homeScreenRows() : FolioSettings::self()->homeScreenColumns();
}

void HomeScreenState::setPageColumns(int pageColumns)
{
    if (m_pageColumns != pageColumns) {
        m_pageColumns = pageColumns;
        Q_EMIT pageColumnsChanged();
    }
}

qreal HomeScreenState::pageViewX() const
{
    return m_pageViewX;
}

void HomeScreenState::setPageViewX(qreal pageViewX)
{
    if (m_pageViewX != pageViewX) {
        m_pageViewX = pageViewX;
        Q_EMIT pageViewXChanged();
    }
}

qreal HomeScreenState::pageWidth() const
{
    return m_pageWidth;
}

void HomeScreenState::setPageWidth(qreal pageWidth)
{
    if (m_pageWidth != pageWidth) {
        m_pageWidth = pageWidth;
        Q_EMIT pageWidthChanged();

        // make sure we snap
        snapPage();
    }
}

qreal HomeScreenState::pageHeight() const
{
    return m_pageHeight;
}

void HomeScreenState::setPageHeight(qreal pageHeight)
{
    if (m_pageHeight != pageHeight) {
        m_pageHeight = pageHeight;
        Q_EMIT pageHeightChanged();
    }
}

qreal HomeScreenState::pageContentWidth() const
{
    return m_pageContentWidth;
}

void HomeScreenState::setPageContentWidth(qreal pageContentWidth)
{
    if (m_pageContentWidth != pageContentWidth) {
        m_pageContentWidth = pageContentWidth;
        Q_EMIT pageContentWidthChanged();
    }
}

qreal HomeScreenState::pageContentHeight() const
{
    return m_pageContentHeight;
}

void HomeScreenState::setPageContentHeight(qreal pageContentHeight)
{
    if (m_pageContentHeight != pageContentHeight) {
        m_pageContentHeight = pageContentHeight;
        Q_EMIT pageContentHeightChanged();
    }
}

qreal HomeScreenState::pageCellWidth() const
{
    return m_pageCellWidth;
}

void HomeScreenState::setPageCellWidth(qreal pageCellWidth)
{
    if (m_pageCellWidth != pageCellWidth) {
        m_pageCellWidth = pageCellWidth;
        Q_EMIT pageCellWidthChanged();
    }
}

qreal HomeScreenState::pageCellHeight() const
{
    return m_pageCellHeight;
}

void HomeScreenState::setPageCellHeight(qreal pageCellHeight)
{
    if (m_pageCellHeight != pageCellHeight) {
        m_pageCellHeight = pageCellHeight;
        Q_EMIT pageCellHeightChanged();
    }
}

qreal HomeScreenState::pageDelegateLabelHeight() const
{
    return m_pageDelegateLabelHeight;
}

void HomeScreenState::setPageDelegateLabelHeight(qreal pageDelegateLabelHeight)
{
    if (m_pageDelegateLabelHeight != pageDelegateLabelHeight) {
        m_pageDelegateLabelHeight = pageDelegateLabelHeight;
        Q_EMIT pageDelegateLabelHeightChanged();
    }
}

qreal HomeScreenState::pageDelegateLabelSpacing() const
{
    return m_pageDelegateLabelSpacing;
}

void HomeScreenState::setPageDelegateLabelSpacing(qreal pageDelegateLabelSpacing)
{
    if (m_pageDelegateLabelSpacing != pageDelegateLabelSpacing) {
        m_pageDelegateLabelSpacing = pageDelegateLabelSpacing;
        Q_EMIT pageDelegateLabelSpacingChanged();
    }
}

qreal HomeScreenState::folderViewX() const
{
    return m_folderViewX;
}

void HomeScreenState::setFolderViewX(qreal folderViewX)
{
    if (m_folderViewX != folderViewX) {
        m_folderViewX = folderViewX;
        Q_EMIT folderViewXChanged();
    }
}

qreal HomeScreenState::folderPageWidth() const
{
    return m_folderPageWidth;
}

void HomeScreenState::setFolderPageWidth(qreal folderPageWidth)
{
    if (m_folderPageWidth != folderPageWidth) {
        m_folderPageWidth = folderPageWidth;
        Q_EMIT folderPageWidthChanged();
    }
}

qreal HomeScreenState::folderPageHeight() const
{
    return m_folderPageHeight;
}

void HomeScreenState::setFolderPageHeight(qreal folderPageHeight)
{
    if (m_folderPageHeight != folderPageHeight) {
        m_folderPageHeight = folderPageHeight;
        Q_EMIT folderPageHeightChanged();
    }
}

qreal HomeScreenState::folderPageContentWidth() const
{
    return m_folderPageContentWidth;
}

void HomeScreenState::setFolderPageContentWidth(qreal folderPageContentWidth)
{
    if (m_folderPageContentWidth != folderPageContentWidth) {
        m_folderPageContentWidth = folderPageContentWidth;
        Q_EMIT folderPageContentWidthChanged();
    }
}

qreal HomeScreenState::folderPageContentHeight() const
{
    return m_folderPageContentHeight;
}

void HomeScreenState::setFolderPageContentHeight(qreal folderPageContentHeight)
{
    if (m_folderPageContentHeight != folderPageContentHeight) {
        m_folderPageContentHeight = folderPageContentHeight;
        Q_EMIT folderPageContentHeightChanged();
    }
}

qreal HomeScreenState::folderOpenProgress() const
{
    return m_folderOpenProgress;
}

void HomeScreenState::setFolderOpenProgress(qreal folderOpenProgress)
{
    if (m_folderOpenProgress != folderOpenProgress) {
        m_folderOpenProgress = folderOpenProgress;
        Q_EMIT folderOpenProgressChanged();
    }
}

FolioApplicationFolder *HomeScreenState::currentFolder() const
{
    return m_currentFolder;
}

void HomeScreenState::setCurrentFolder(FolioApplicationFolder *folder)
{
    if (m_currentFolder != folder) {
        m_currentFolder = folder;
        Q_EMIT currentFolderChanged();
    }
}

qreal HomeScreenState::appDrawerOpenProgress()
{
    return m_appDrawerOpenProgress;
}

qreal HomeScreenState::appDrawerY()
{
    return m_appDrawerY;
}

void HomeScreenState::setAppDrawerY(qreal appDrawerY)
{
    m_appDrawerY = appDrawerY;
    m_appDrawerOpenProgress = 1 - std::min(std::max(m_appDrawerY, 0.0), APP_DRAWER_OPEN_DIST) / APP_DRAWER_OPEN_DIST;
    Q_EMIT appDrawerYChanged();
    Q_EMIT appDrawerOpenProgressChanged();
}

qreal HomeScreenState::searchWidgetOpenProgress()
{
    return m_searchWidgetOpenProgress;
}

qreal HomeScreenState::searchWidgetY()
{
    return m_searchWidgetOpenProgress;
}

void HomeScreenState::setSearchWidgetY(qreal searchWidgetY)
{
    m_searchWidgetY = searchWidgetY;
    m_searchWidgetOpenProgress = 1 - std::min(std::max(m_searchWidgetY, 0.0), SEARCH_WIDGET_OPEN_DIST) / SEARCH_WIDGET_OPEN_DIST;
    Q_EMIT searchWidgetYChanged();
    Q_EMIT searchWidgetOpenProgressChanged();
}

qreal HomeScreenState::delegateDragX()
{
    return m_delegateDragX;
}

void HomeScreenState::setDelegateDragX(qreal delegateDragX)
{
    m_delegateDragX = delegateDragX;
    Q_EMIT delegateDragXChanged();
}

qreal HomeScreenState::delegateDragY()
{
    return m_delegateDragY;
}

void HomeScreenState::setDelegateDragY(qreal delegateDragY)
{
    m_delegateDragY = delegateDragY;
    Q_EMIT delegateDragYChanged();
}

int HomeScreenState::currentPage()
{
    return m_pageNum;
}

void HomeScreenState::setCurrentPage(int currentPage)
{
    if (m_pageNum != currentPage) {
        m_pageNum = currentPage;
        Q_EMIT pageNumChanged();
    }
}

int HomeScreenState::currentFolderPage()
{
    return m_folderPageNum;
}

void HomeScreenState::openAppDrawer()
{
    cancelAppDrawerAnimations();
    m_openAppDrawerAnim->setStartValue(m_appDrawerY);
    m_openAppDrawerAnim->start();
}

void HomeScreenState::closeAppDrawer()
{
    cancelAppDrawerAnimations();
    m_closeAppDrawerAnim->setStartValue(m_appDrawerY);
    m_closeAppDrawerAnim->start();
}

void HomeScreenState::openSearchWidget()
{
    cancelSearchWidgetAnimations();
    m_openSearchWidgetAnim->setStartValue(m_searchWidgetY);
    m_openSearchWidgetAnim->start();
}

void HomeScreenState::closeSearchWidget()
{
    cancelSearchWidgetAnimations();
    m_closeSearchWidgetAnim->setStartValue(m_searchWidgetY);
    m_closeSearchWidgetAnim->start();
}

void HomeScreenState::snapPage()
{
    int numOfPages = PageListModel::self()->rowCount();

    int leftPage = std::max((qreal)0, std::min((qreal)numOfPages - 1, m_pageViewX / m_pageWidth));
    qreal leftPagePos = -leftPage * m_pageWidth;

    if (leftPage == numOfPages + 1) {
        // if we are past the last page
        goToPage(leftPage);
    } else {
        qreal rightPagePos = leftPagePos - m_pageWidth;

        // go to the closer page (right or left)
        if (qAbs(rightPagePos - m_pageViewX) < qAbs(leftPagePos - m_pageViewX)) {
            goToPage(leftPage + 1);
        } else {
            goToPage(leftPage);
        }
    }
}

void HomeScreenState::goToPage(int page)
{
    if (page < 0) {
        page = 0;
    }

    int numOfPages = PageListModel::self()->rowCount();
    if (page >= numOfPages) {
        page = std::max(0, numOfPages - 1);
    }

    setCurrentPage(page);

    m_pageAnim->setStartValue(m_pageViewX);
    m_pageAnim->setEndValue(-page * m_pageWidth);
    m_pageAnim->start();
}

void HomeScreenState::goToFolderPage(int page)
{
    if (!m_currentFolder) {
        return;
    }

    if (page < 0) {
        page = 0;
    }

    int numOfPages = m_currentFolder->applications()->numTotalPages();
    if (page >= numOfPages) {
        page = std::max(0, numOfPages - 1);
    }

    m_folderPageNum = page;
    Q_EMIT folderPageNumChanged();

    m_folderPageAnim->setStartValue(m_folderViewX);
    m_folderPageAnim->setEndValue(-page * m_folderPageWidth);
    m_folderPageAnim->start();
}

void HomeScreenState::openFolder(FolioApplicationFolder *folder)
{
    setCurrentFolder(folder);

    m_openFolderAnim->stop();
    m_closeFolderAnim->stop();
    m_openFolderAnim->setStartValue(m_folderOpenProgress);
    m_openFolderAnim->start();
}

void HomeScreenState::closeFolder()
{
    m_openFolderAnim->stop();
    m_closeFolderAnim->stop();
    m_closeFolderAnim->setStartValue(m_folderOpenProgress);
    m_closeFolderAnim->start();
}

void HomeScreenState::startDelegateDrag(qreal startX, qreal startY)
{
    // start drag and drop positions
    setDelegateDragX(startX);
    setDelegateDragY(startY);

    // end current swipe
    swipeEnded();

    // start the delegate drag
    setSwipeState(SwipeState::AwaitingDraggingDelegate);
}

void HomeScreenState::startDelegatePageDrag(qreal startX, qreal startY, int page, int row, int column)
{
    startDelegateDrag(startX, startY);
    Q_EMIT delegateDragFromPageStarted(page, row, column);
}

void HomeScreenState::startDelegateFavouritesDrag(qreal startX, qreal startY, int position)
{
    startDelegateDrag(startX, startY);
    Q_EMIT delegateDragFromFavouritesStarted(position);
}

void HomeScreenState::startDelegateAppDrawerDrag(qreal startX, qreal startY, QString storageId)
{
    startDelegateDrag(startX, startY);
    // we start dragging the delegate immediately from the app drawer
    // because we don't have a context menu to deal with
    setSwipeState(SwipeState::DraggingDelegate);
    Q_EMIT delegateDragFromAppDrawerStarted(storageId);
}

void HomeScreenState::startDelegateFolderDrag(qreal startX, qreal startY, FolioApplicationFolder *folder, int position)
{
    startDelegateDrag(startX, startY);
    Q_EMIT delegateDragFromFolderStarted(folder, position);
}

void HomeScreenState::cancelDelegateDrag()
{
    swipeEnded();
}

void HomeScreenState::swipeStarted()
{
    if (m_swipeState != SwipeState::None) {
        return;
    }

    setSwipeState(SwipeState::DeterminingSwipeType);
}

void HomeScreenState::swipeEnded()
{
    switch (m_swipeState) {
    case SwipeState::SwipingOpenAppDrawer:
    case SwipeState::SwipingCloseAppDrawer:
        if (m_movingUp) {
            closeAppDrawer();
        } else {
            openAppDrawer();
        }
        break;
    case SwipeState::SwipingOpenSearchWidget:
    case SwipeState::SwipingCloseSearchWidget:
        if (m_movingUp) {
            openSearchWidget();
        } else {
            closeSearchWidget();
        }
        break;
    case SwipeState::SwipingPages: {
        int page = std::max(0.0, -m_pageViewX) / m_pageWidth;

        // m_movingRight refers to finger movement
        if (m_movingRight || m_pageViewX > 0) {
            goToPage(page);
        } else {
            goToPage(page + 1);
        }
        break;
    }
    case SwipeState::SwipingFolderPages: {
        int page = std::max(0.0, -m_folderViewX) / m_folderPageWidth;

        // m_movingRight refers to finger movement
        if (m_movingRight || m_folderViewX > 0) {
            goToFolderPage(page);
        } else {
            goToFolderPage(page + 1);
        }
        break;
    }
    case SwipeState::DraggingDelegate:
        Q_EMIT delegateDragEnded();
        break;
    case SwipeState::AwaitingDraggingDelegate:
    case SwipeState::DeterminingSwipeType:
        break;
    default:
        break;
    }

    setSwipeState(SwipeState::None);
}

void HomeScreenState::swipeMoved(qreal totalDeltaX, qreal totalDeltaY, qreal deltaX, qreal deltaY)
{
    m_movingUp = deltaY > 0;

    switch (m_swipeState) {
    case SwipeState::DeterminingSwipeType:
        // check if we can determine the type of swipe this is
        determineSwipeTypeAfterThreshold(totalDeltaX, totalDeltaY);
        break;
    case SwipeState::SwipingOpenSearchWidget:
    case SwipeState::SwipingCloseSearchWidget:
        setSearchWidgetY(m_searchWidgetY - deltaY);
        break;
    case SwipeState::SwipingOpenAppDrawer:
    case SwipeState::SwipingCloseAppDrawer:
        setAppDrawerY(m_appDrawerY + deltaY);
        break;
    case SwipeState::SwipingPages:
        m_movingRight = deltaX > 0;
        setPageViewX(m_pageViewX + deltaX);
        break;
    case SwipeState::SwipingFolderPages:
        m_movingRight = deltaX > 0;
        setFolderViewX(m_folderViewX + deltaX);
        break;
    case SwipeState::AwaitingDraggingDelegate:
        setSwipeState(SwipeState::DraggingDelegate);
        break;
    case SwipeState::DraggingDelegate:
        setDelegateDragX(m_delegateDragX + deltaX);
        setDelegateDragY(m_delegateDragY + deltaY);
        break;
    default:
        break;
    }
}

void HomeScreenState::determineSwipeTypeAfterThreshold(qreal totalDeltaX, qreal totalDeltaY)
{
    // we check if the x or y movement has passed a certain threshold before determining the swipe type

    if (qAbs(totalDeltaX) >= DETERMINE_SWIPE_THRESHOLD && m_viewState == ViewState::PageView) {
        // select horizontal swipe mode (only if in page view)
        setSwipeState(SwipeState::SwipingPages);

        // ensure no animations are running when starting a swipe
        m_pageAnim->stop();

    } else if (qAbs(totalDeltaX) >= DETERMINE_SWIPE_THRESHOLD && m_viewState == ViewState::FolderView) {
        // select horizontal swipe mode (only if in page view)
        setSwipeState(SwipeState::SwipingFolderPages);

        // ensure no animations are running when starting a swipe
        m_folderPageAnim->stop();

    } else if (qAbs(totalDeltaY) >= DETERMINE_SWIPE_THRESHOLD) {
        // select vertical swipe mode

        if (m_movingUp) {
            // moving up
            switch (m_viewState) {
            case ViewState::PageView:
                // if the app drawer is still being opened
                if (m_openAppDrawerAnim->state() == QPropertyAnimation::Running) {
                    setSwipeState(SwipeState::SwipingOpenAppDrawer);
                    cancelAppDrawerAnimations();
                } else {
                    setSwipeState(SwipeState::SwipingOpenSearchWidget);
                    cancelSearchWidgetAnimations();
                }
                break;
            case ViewState::AppDrawerView:
                setSwipeState(SwipeState::SwipingCloseAppDrawer);
                cancelAppDrawerAnimations();
                break;
            case ViewState::SearchWidgetView:
                setSwipeState(SwipeState::SwipingCloseSearchWidget);
                cancelSearchWidgetAnimations();
            case ViewState::FolderView:
                // no vertical behaviour in folder view
            default:
                break;
            }
        } else {
            // moving down
            switch (m_viewState) {
            case ViewState::PageView:
                if (m_openSearchWidgetAnim->state() == QPropertyAnimation::Running) {
                    setSwipeState(SwipeState::SwipingOpenSearchWidget);
                    cancelSearchWidgetAnimations();
                } else {
                    setSwipeState(SwipeState::SwipingOpenAppDrawer);
                    cancelAppDrawerAnimations();
                }
                break;
            case ViewState::SearchWidgetView:
                setSwipeState(SwipeState::SwipingCloseSearchWidget);
                cancelSearchWidgetAnimations();
                break;
            case ViewState::AppDrawerView:
                setSwipeState(SwipeState::SwipingCloseAppDrawer);
                cancelAppDrawerAnimations();
            case ViewState::FolderView:
                // no vertical behaviour in folder view
            default:
                break;
            }
        }
    }
}

void HomeScreenState::cancelAppDrawerAnimations()
{
    m_openAppDrawerAnim->stop();
    m_closeAppDrawerAnim->stop();
}

void HomeScreenState::cancelSearchWidgetAnimations()
{
    m_openSearchWidgetAnim->stop();
    m_closeSearchWidgetAnim->stop();
}

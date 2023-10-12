// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "dragstate.h"
#include "favouritesmodel.h"
#include "foliosettings.h"
#include "pagelistmodel.h"

#include <KLocalizedString>
#include <algorithm>

// TODO don't hardcode, use page widths
const int PAGE_CHANGE_THRESHOLD = 30;

DelegateDragPosition::DelegateDragPosition(QObject *parent)
    : QObject{parent}
{
}

DelegateDragPosition::~DelegateDragPosition() = default;

void DelegateDragPosition::copyFrom(DelegateDragPosition *position)
{
    setPage(position->page());
    setPageRow(position->pageRow());
    setPageColumn(position->pageColumn());
    setFavouritesPosition(position->favouritesPosition());
    setLocation(position->location());
}

DelegateDragPosition::Location DelegateDragPosition::location() const
{
    return m_location;
}

void DelegateDragPosition::setLocation(Location location)
{
    if (m_location != location) {
        m_location = location;
        Q_EMIT locationChanged();
    }
}

int DelegateDragPosition::page() const
{
    return m_page;
}

void DelegateDragPosition::setPage(int page)
{
    if (m_page != page) {
        m_page = page;
        Q_EMIT pageChanged();
    }
}

int DelegateDragPosition::pageRow() const
{
    return m_pageRow;
}

void DelegateDragPosition::setPageRow(int pageRow)
{
    if (m_pageRow != pageRow) {
        m_pageRow = pageRow;
        Q_EMIT pageRowChanged();
    }
}

int DelegateDragPosition::pageColumn() const
{
    return m_pageColumn;
}

void DelegateDragPosition::setPageColumn(int pageColumn)
{
    if (m_pageColumn != pageColumn) {
        m_pageColumn = pageColumn;
        Q_EMIT pageColumnChanged();
    }
}

int DelegateDragPosition::favouritesPosition() const
{
    return m_favouritesPosition;
}

void DelegateDragPosition::setFavouritesPosition(int favouritesPosition)
{
    if (m_favouritesPosition != favouritesPosition) {
        m_favouritesPosition = favouritesPosition;
        Q_EMIT favouritesPositionChanged();
    }
}

DragState::DragState(HomeScreenState *state, QObject *parent)
    : QObject{parent}
    , m_changePageTimer{new QTimer{this}}
    , m_favouritesInsertBetweenTimer{new QTimer{this}}
    , m_state{state}
    , m_candidateDropPosition{new DelegateDragPosition{this}}
    , m_startPosition{new DelegateDragPosition{this}}
{
    if (!state) {
        return;
    }

    // 500 ms hold before page timer changes
    m_changePageTimer->setInterval(500);
    m_changePageTimer->setSingleShot(true);

    m_favouritesInsertBetweenTimer->setInterval(500);
    m_favouritesInsertBetweenTimer->setSingleShot(true);

    connect(m_changePageTimer, &QTimer::timeout, this, &DragState::onChangePageTimerFinished);
    connect(m_favouritesInsertBetweenTimer, &QTimer::timeout, this, &DragState::onFavouritesInsertBetweenTimerFinished);

    connect(m_state, &HomeScreenState::delegateDragFromPageStarted, this, &DragState::onDelegateDragFromPageStarted);
    connect(m_state, &HomeScreenState::delegateDragFromAppDrawerStarted, this, &DragState::onDelegateDragFromAppDrawerStarted);
    connect(m_state, &HomeScreenState::delegateDragFromFavouritesStarted, this, &DragState::onDelegateDragFromFavouritesStarted);

    connect(m_state, &HomeScreenState::delegateDragEnded, this, &DragState::onDelegateDropped);

    connect(m_state, &HomeScreenState::pageNumChanged, this, [this]() {
        m_candidateDropPosition->setPageRow(m_state->currentPage());
    });

    connect(m_state, &HomeScreenState::delegateDragXChanged, this, &DragState::onDelegateDragPositionChanged);
    connect(m_state, &HomeScreenState::delegateDragYChanged, this, &DragState::onDelegateDragPositionChanged);
}

DelegateDragPosition *DragState::candidateDropPosition() const
{
    return m_candidateDropPosition;
}

DelegateDragPosition *DragState::startPosition() const
{
    return m_startPosition;
}

void DragState::onFavouritesInsertBetweenTimerFinished()
{
    qDebug() << "favourites inserttimer finished" << m_favouritesInsertBetweenIndex;

    // update the candidate drop position
    m_candidateDropPosition->setFavouritesPosition(m_favouritesInsertBetweenIndex);
    m_candidateDropPosition->setLocation(DelegateDragPosition::Favourites);

    if (isStartPositionEqualDropPosition()) {
        return;
    }

    // insert it at this position, shifting existing apps to the side
    FavouritesModel::self()->setGhostEntry(m_favouritesInsertBetweenIndex);
}

void DragState::onDelegateDragPositionChanged()
{
    if (!m_state) {
        return;
    }

    // we want to update the candidate drop position variable in this function!

    qreal x = getDraggedDelegateX();
    qreal y = getDraggedDelegateY();

    bool inFavouritesArea = y > m_state->pageHeight();

    // stop the favourites insertion timer if the delegate has moved out
    if (!inFavouritesArea) {
        m_favouritesInsertBetweenTimer->stop();
    }

    if (inFavouritesArea) {
        // we are in the favourites bar area

        int dropIndex = FavouritesModel::self()->dropInsertPosition(x);

        qDebug() << "in favourites";

        // if the delegate has moved to another position, cancel the insert timer
        if (dropIndex != m_favouritesInsertBetweenIndex) {
            qDebug() << "cancel timer" << dropIndex << m_favouritesInsertBetweenIndex;
            m_favouritesInsertBetweenTimer->stop();
        }

        // if we need to make space for the delegate
        if (FavouritesModel::self()->dropPositionIsEdge(x)) {
            qDebug() << "at edge" << dropIndex;
            // start the insertion timer (so that the user has time to move the delegate away)
            if (!m_favouritesInsertBetweenTimer->isActive()) {
                m_favouritesInsertBetweenTimer->start();
                m_favouritesInsertBetweenIndex = dropIndex;
            }
        } else {
            qDebug() << "hovering over";
            // if we are hovering over the center of a folder or app

            // update the current drop position
            m_candidateDropPosition->setFavouritesPosition(dropIndex);
            m_candidateDropPosition->setLocation(DelegateDragPosition::Favourites);

            // delete ghost entry when hovering over a folder
            if (FavouritesModel::self()->getGhostEntryPosition() != dropIndex) {
                FavouritesModel::self()->deleteGhostEntry();
            }
        }
    } else {
        // we are in the homescreen pages area
        int page = m_state->currentPage();

        // calculate the row and column the delegate is at
        qreal pageHorizontalMargin = (m_state->pageWidth() - m_state->pageContentWidth()) / 2;
        qreal pageVerticalMargin = (m_state->pageHeight() - m_state->pageContentHeight()) / 2;
        qreal cellWidth = m_state->pageCellWidth();
        qreal cellHeight = m_state->pageCellHeight();

        int row = (y - pageVerticalMargin) / cellHeight;
        int column = (x - pageHorizontalMargin) / cellWidth;

        // ensure it's in bounds
        row = std::max(0, std::min(FolioSettings::self()->homeScreenRows() - 1, row));
        column = std::max(0, std::min(FolioSettings::self()->homeScreenColumns() - 1, column));

        // update the current drop position
        m_candidateDropPosition->setPage(page);
        m_candidateDropPosition->setPageRow(row);
        m_candidateDropPosition->setPageColumn(column);
        m_candidateDropPosition->setLocation(DelegateDragPosition::Pages);
    }

    const int leftPagePosition = 0;
    const int rightPagePosition = m_state->pageWidth();

    // determine if the delegate is near the edge of a page (to switch pages).
    // -> start the change page timer if we at the page edge.
    if (qAbs(leftPagePosition - x) <= PAGE_CHANGE_THRESHOLD || qAbs(rightPagePosition - x) <= PAGE_CHANGE_THRESHOLD) {
        if (!m_changePageTimer->isActive()) {
            m_changePageTimer->start();
        }
    } else {
        if (m_changePageTimer->isActive()) {
            m_changePageTimer->stop();
        }
    }
}

void DragState::onDelegateDragFromPageStarted(int page, int row, int column)
{
    // fetch delegate at start position
    PageModel *pageModel = PageListModel::self()->getPage(page);
    if (pageModel) {
        m_dropDelegate = pageModel->getDelegate(row, column);
    } else {
        m_dropDelegate = nullptr;
    }

    m_startPosition->setPage(page);
    m_startPosition->setPageRow(row);
    m_startPosition->setPageColumn(column);
    m_startPosition->setLocation(DelegateDragPosition::Pages);
}

void DragState::onDelegateDragFromFavouritesStarted(int position)
{
    // fetch delegate at start position
    m_dropDelegate = FavouritesModel::self()->getEntryAt(position);
    m_startPosition->setFavouritesPosition(position);
    m_startPosition->setLocation(DelegateDragPosition::Favourites);

    // set the position to be invisible
    FavouritesModel::self()->setInvisiblePosition(position);
}

void DragState::onDelegateDragFromAppDrawerStarted(QString storageId)
{
    // create delegate to be dropped
    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        FolioApplication *app = new FolioApplication{this, service};
        m_dropDelegate = new FolioDelegate{app, this};
    } else {
        m_dropDelegate = nullptr;
    }

    m_startPosition->setLocation(DelegateDragPosition::AppDrawer);
}

void DragState::onDelegateDropped()
{
    qDebug() << "dropped" << m_dropDelegate;
    if (!m_dropDelegate) {
        return;
    }

    // add dropped delegate
    createDropPositionDelegate();

    // remove old delegate
    if (!isStartPositionEqualDropPosition()) {
        deleteStartPositionDelegate();
    }

    // delete empty pages at the end if they exist
    // (it can be created if user drags app to new page, but doesn't place it there)
    while (PageListModel::self()->isLastPageEmpty() && PageListModel::self()->rowCount() > 1) {
        PageListModel::self()->removePage(PageListModel::self()->rowCount() - 1);
    }

    // clear ghost position if there is one
    FavouritesModel::self()->clearInvisiblePosition();
    FavouritesModel::self()->deleteGhostEntry();

    // reset timers
    m_changePageTimer->stop();
    m_favouritesInsertBetweenTimer->stop();
}

void DragState::onChangePageTimerFinished()
{
    if (!m_state) {
        return;
    }

    const int leftPagePosition = 0;
    const int rightPagePosition = m_state->pageWidth();

    qreal x = getDraggedDelegateX();
    if (qAbs(leftPagePosition - x) <= PAGE_CHANGE_THRESHOLD) {
        // if we are at the left edge, go left
        int page = m_state->currentPage() - 1;
        if (page >= 0) {
            m_state->goToPage(page);
        }

    } else if (qAbs(rightPagePosition - x) <= PAGE_CHANGE_THRESHOLD) {
        // if we are at the right edge, go right
        int page = m_state->currentPage() + 1;

        // if we are at the right-most page, try to create a new one if the current page isn't empty
        if (page == PageListModel::self()->rowCount() && !PageListModel::self()->isLastPageEmpty()) {
            PageListModel::self()->addPageAtEnd();
        }

        // go to page if it exists
        if (page < PageListModel::self()->rowCount()) {
            m_state->goToPage(page);
        }
    }
}

void DragState::deleteStartPositionDelegate()
{
    // delete the delegate at the start position
    switch (m_startPosition->location()) {
    case DelegateDragPosition::Pages: {
        PageModel *page = PageListModel::self()->getPage(m_startPosition->page());
        if (page) {
            page->removeDelegate(m_startPosition->pageRow(), m_startPosition->pageColumn());
        }
        break;
    }
    case DelegateDragPosition::Favourites:
        FavouritesModel::self()->removeEntry(m_startPosition->favouritesPosition());
        break;
    case DelegateDragPosition::AppDrawer:
    default:
        break;
    }
}

// TODO remove modifyFolders, unnecessary
void DragState::createDropPositionDelegate()
{
    qDebug() << "drop location:" << m_candidateDropPosition->location();

    // creates the delegate at the drop position
    switch (m_candidateDropPosition->location()) {
    case DelegateDragPosition::Pages: {
        qDebug() << "drop at page" << m_candidateDropPosition->page() << m_candidateDropPosition->pageRow() << m_candidateDropPosition->pageColumn();

        // locate the page we are dropping on
        PageModel *page = PageListModel::self()->getPage(m_candidateDropPosition->page());
        if (!page) {
            break;
        }

        int row = m_candidateDropPosition->pageRow();
        int column = m_candidateDropPosition->pageColumn();

        // delegate to add
        FolioPageDelegate *delegate = new FolioPageDelegate{row, column, m_dropDelegate, page};

        // delegate that exists at the drop position
        FolioPageDelegate *existingDelegate = page->getDelegate(row, column);

        // if a delegate already exists at the spot, check if we can insert/create a folder
        if (existingDelegate) {
            if (delegate->type() == FolioDelegate::Application) {
                if (existingDelegate->type() == FolioDelegate::Folder) {
                    // add the app to the existing folder

                    auto existingFolder = existingDelegate->folder();
                    existingFolder->addApp(delegate->application()->storageId(), existingFolder->applications()->rowCount());
                    delegate->deleteLater();

                    break;
                } else if (existingDelegate->type() == FolioDelegate::Application && !isStartPositionEqualDropPosition()) {
                    // create a folder from the two apps

                    FolioApplicationFolder *folder = new FolioApplicationFolder(this, i18n("Folder")); // TODO folder name
                    folder->addApp(delegate->application()->storageId(), 0);
                    folder->addApp(existingDelegate->application()->storageId(), 0);
                    FolioPageDelegate *folderDelegate = new FolioPageDelegate{row, column, folder, this};

                    page->removeDelegate(row, column);
                    page->addDelegate(folderDelegate);

                    break;
                }
            }
        }

        // default behavior for folders or dropping an app at an empty spot

        bool added = page->addDelegate(delegate);

        // if we couldn't add the delegate, try again but at the start position (return to start)
        if (!added && !isStartPositionEqualDropPosition()) {
            m_candidateDropPosition->copyFrom(m_startPosition);
            createDropPositionDelegate();
        }
        break;
    }
    case DelegateDragPosition::Favourites: {
        qDebug() << "creating favourite at" << m_candidateDropPosition->favouritesPosition();

        // delegate that exists at the drop position
        FolioDelegate *existingDelegate = FavouritesModel::self()->getEntryAt(m_candidateDropPosition->favouritesPosition());

        // if a delegate already exists at the spot, check if we can insert/create a folder
        if (existingDelegate) {
            if (m_dropDelegate->type() == FolioDelegate::Application) {
                if (existingDelegate->type() == FolioDelegate::Folder) {
                    // add the app to the existing folder

                    auto existingFolder = existingDelegate->folder();
                    existingFolder->addApp(m_dropDelegate->application()->storageId(), existingFolder->applications()->rowCount());
                    m_dropDelegate->deleteLater();

                    break;
                } else if (existingDelegate->type() == FolioDelegate::Application && !isStartPositionEqualDropPosition()) {
                    // create a folder from the two apps

                    FolioApplicationFolder *folder = new FolioApplicationFolder(this, i18n("Folder")); // TODO folder name
                    folder->addApp(m_dropDelegate->application()->storageId(), 0);
                    folder->addApp(existingDelegate->application()->storageId(), 0);
                    FolioDelegate *folderDelegate = new FolioDelegate{folder, this};

                    FavouritesModel::self()->removeEntry(m_candidateDropPosition->favouritesPosition());
                    FavouritesModel::self()->addEntry(m_candidateDropPosition->favouritesPosition(), folderDelegate);

                    break;
                }
            }
        }

        // otherwise, just add the delegate at this position

        bool added = FavouritesModel::self()->addEntry(m_candidateDropPosition->favouritesPosition(), m_dropDelegate);

        // if we couldn't add the delegate, try again but at the start position
        if (!added && !isStartPositionEqualDropPosition()) {
            m_candidateDropPosition->copyFrom(m_startPosition);
            createDropPositionDelegate();
        }

        // correct position when we delete from an entry earlier in the favourites
        if (added) {
            if (m_startPosition->location() == DelegateDragPosition::Favourites
                && m_startPosition->favouritesPosition() > m_candidateDropPosition->favouritesPosition()) {
                m_startPosition->setFavouritesPosition(m_startPosition->favouritesPosition() - 1);
            }
        }
        break;
    }
    case DelegateDragPosition::AppDrawer:
    default:
        break;
    }
}

bool DragState::isStartPositionEqualDropPosition()
{
    return m_startPosition->location() == m_candidateDropPosition->location() && m_startPosition->page() == m_candidateDropPosition->page()
        && m_startPosition->pageRow() == m_candidateDropPosition->pageRow() && m_startPosition->pageColumn() == m_candidateDropPosition->pageColumn()
        && m_startPosition->favouritesPosition() == m_candidateDropPosition->favouritesPosition();
}

qreal DragState::getDraggedDelegateX()
{
    // adjust to get the position of the center of the delegate
    return m_state->delegateDragX() + m_state->pageCellWidth() / 2;
}

qreal DragState::getDraggedDelegateY()
{
    // adjust to get the position of the center of the delegate
    return m_state->delegateDragY() + m_state->pageCellHeight() / 2;
}

// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "dragstate.h"
#include "favouritesmodel.h"
#include "foliosettings.h"
#include "pagelistmodel.h"

#include <algorithm>

// TODO don't hardcode, use page widths
const int PAGE_CHANGE_THRESHOLD = 30;

DelegateDragPosition::DelegateDragPosition(QObject *parent)
    : QObject{parent}
{
}

DelegateDragPosition::~DelegateDragPosition() = default;

DelegateDragPosition::Location DelegateDragPosition::location() const
{
    return m_location;
}

void DelegateDragPosition::setLocation(Location location)
{
    m_location = location;
    Q_EMIT locationChanged();
}

int DelegateDragPosition::page() const
{
    return m_page;
}

void DelegateDragPosition::setPage(int page)
{
    m_page = page;
    Q_EMIT pageChanged();
}

int DelegateDragPosition::pageRow() const
{
    return m_pageRow;
}

void DelegateDragPosition::setPageRow(int pageRow)
{
    m_pageRow = pageRow;
    Q_EMIT pageRowChanged();
}

int DelegateDragPosition::pageColumn() const
{
    return m_pageColumn;
}

void DelegateDragPosition::setPageColumn(int pageColumn)
{
    m_pageColumn = pageColumn;
    Q_EMIT pageColumnChanged();
}

int DelegateDragPosition::favouritesPosition() const
{
    return m_favouritesPosition;
}

void DelegateDragPosition::setFavouritesPosition(int favouritesPosition)
{
    m_favouritesPosition = favouritesPosition;
    Q_EMIT favouritesPositionChanged();
}

DragState::DragState(HomeScreenState *state, QObject *parent)
    : QObject{parent}
    , m_changePageTimer{new QTimer{this}}
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

    connect(m_changePageTimer, &QTimer::timeout, this, &DragState::onChangePageTimerFinished);

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

void DragState::onDelegateDragPositionChanged()
{
    if (!m_state) {
        return;
    }

    // we need to update the candidate drop position
    qreal x = getDraggedDelegateX();
    qreal y = getDraggedDelegateY();

    if (y > m_state->pageHeight()) {
        // we are in the favourites bar area

        // update the current drop position
        m_candidateDropPosition->setFavouritesPosition(0); // TODO positioning
        m_candidateDropPosition->setLocation(DelegateDragPosition::Favourites);
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

        row = std::max(0, std::min(FolioSettings::self()->homeScreenRows() - 1, row));
        column = std::max(0, std::min(FolioSettings::self()->homeScreenColumns() - 1, column));

        // update the current drop position
        m_candidateDropPosition->setPage(page);
        m_candidateDropPosition->setPageRow(row);
        m_candidateDropPosition->setPageColumn(column);
        m_candidateDropPosition->setLocation(DelegateDragPosition::Pages);
    }

    // determine if the delegate is near the edge of a page (to switch pages)
    // start the change page timer if we are
    if (qAbs(getLeftPagePosition() - x) <= PAGE_CHANGE_THRESHOLD || qAbs(getRightPagePosition() - x) <= PAGE_CHANGE_THRESHOLD) {
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
    if (!m_dropDelegate) {
        return;
    }

    qDebug() << "delegate drop" << m_candidateDropPosition->location();

    // remove old delegate
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

    // add dropped delegate
    switch (m_candidateDropPosition->location()) {
    case DelegateDragPosition::Pages: {
        PageModel *page = PageListModel::self()->getPage(m_candidateDropPosition->page());
        if (page) {
            FolioPageDelegate *delegate =
                new FolioPageDelegate{m_candidateDropPosition->pageRow(), m_candidateDropPosition->pageColumn(), m_dropDelegate, page};
            page->addDelegate(delegate);
        }
        break;
    }
    case DelegateDragPosition::Favourites:
        FavouritesModel::self()->addEntry(m_candidateDropPosition->favouritesPosition(), m_dropDelegate);
        break;
    case DelegateDragPosition::AppDrawer:
    default:
        break;
    }
}

void DragState::onChangePageTimerFinished()
{
    if (!m_state) {
        return;
    }

    // we don't use m_state->pageNum() because that may differ from the current page position
    // as it changes prior to animations fully finishing

    qreal x = getDraggedDelegateX();
    if (qAbs(getLeftPagePosition() - x) <= PAGE_CHANGE_THRESHOLD) {
        int page = getLeftPagePosition() / m_state->pageWidth();
        if (page >= 0) {
            m_state->goToPage(page);
        }
    } else if (qAbs(getRightPagePosition() - x) <= PAGE_CHANGE_THRESHOLD) {
        int page = getRightPagePosition() / m_state->pageWidth();
        if (page < PageListModel::self()->rowCount()) {
            m_state->goToPage(page);
        }
    }
}

qreal DragState::getLeftPagePosition()
{
    qreal x = -m_state->delegateDragX();
    int leftPositionalPage = std::max(0.0, x / m_state->pageWidth());

    return leftPositionalPage * m_state->pageWidth();
}

qreal DragState::getRightPagePosition()
{
    return getLeftPagePosition() + m_state->pageWidth();
}

qreal DragState::getDraggedDelegateX()
{
    return m_state->delegateDragX() + m_state->pageCellWidth() / 2;
}

qreal DragState::getDraggedDelegateY()
{
    return m_state->delegateDragY() + m_state->pageCellHeight() / 2;
}

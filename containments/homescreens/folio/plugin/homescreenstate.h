// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "qqml.h"
#include <QObject>
#include <QPropertyAnimation>

// TODO ISSUES:
// - if we swipe up while the search widget is opening, the app drawer opens too
//   - we might need to add secondary states in between when these animations are running
//   - or check if these animations are running, and resume state from there

class HomeScreenState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(HomeScreenState::SwipeState swipeState READ swipeState NOTIFY swipeStateChanged)
    Q_PROPERTY(HomeScreenState::ViewState viewState READ viewState NOTIFY viewStateChanged)
    Q_PROPERTY(qreal pageViewX READ pageViewX WRITE setPageViewX NOTIFY pageViewXChanged)
    Q_PROPERTY(qreal pageWidth READ pageWidth WRITE setPageWidth NOTIFY pageWidthChanged)

    Q_PROPERTY(qreal appDrawerOpenProgress READ appDrawerOpenProgress NOTIFY appDrawerOpenProgressChanged)
    Q_PROPERTY(qreal appDrawerY READ appDrawerY WRITE setAppDrawerY NOTIFY appDrawerYChanged)

    Q_PROPERTY(qreal searchWidgetOpenProgress READ searchWidgetOpenProgress NOTIFY searchWidgetOpenProgressChanged)
    Q_PROPERTY(qreal searchWidgetY READ searchWidgetY WRITE setSearchWidgetY NOTIFY searchWidgetYChanged)

    Q_PROPERTY(QQmlListProperty<QObject> children READ children CONSTANT)
    Q_CLASSINFO("DefaultProperty", "children")
    QML_NAMED_ELEMENT("HomeScreenState")

public:
    enum SwipeState {
        None,
        DeterminingSwipeType, // TODO
        SwipingPages,
        SwipingOpenAppDrawer,
        SwipingCloseAppDrawer,
        SwipingOpenSearchWidget,
        SwipingCloseSearchWidget,
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

    // the current horizontal position of the pageview
    // starts at 0, each page is m_pageWidth wide
    // first page is at -m_pageWidth, second is at -m_pageWidth * 2, etc.
    qreal pageViewX();
    void setPageViewX(qreal pageViewX);

    // the width of a single pageview page (set from QML)
    qreal pageWidth();
    void setPageWidth(qreal pageWidth);

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

    QQmlListProperty<QObject> children();

Q_SIGNALS:
    void swipeStateChanged();
    void viewStateChanged();
    void pageViewXChanged();
    void pageWidthChanged();
    void appDrawerOpenProgressChanged();
    void appDrawerYChanged();
    void searchWidgetOpenProgressChanged();
    void searchWidgetYChanged();

public Q_SLOTS:
    void openAppDrawer();
    void closeAppDrawer();
    void openSearchWidget();
    void closeSearchWidget();
    void snapPage(); // snaps to closest page
    void goToPage(int page);

    // from SwipeArea
    void swipeStarted();
    void swipeEnded();
    void swipeMoved(qreal totalDeltaX, qreal totalDeltaY, qreal deltaX, qreal deltaY);

private:
    void setViewState(ViewState viewState);
    void setSwipeState(SwipeState swipeState);
    void cancelAppDrawerAnimations();
    void cancelSearchWidgetAnimations();

    // check if we passed the swipe threshold, and determine the swipe type after
    void determineSwipeTypeAfterThreshold(qreal totalDeltaX, qreal totalDeltaY);

    SwipeState m_swipeState{SwipeState::None};
    ViewState m_viewState{ViewState::PageView};

    qreal m_pageViewX{0};
    qreal m_pageWidth{0};
    qreal m_appDrawerOpenProgress{0};
    qreal m_appDrawerY{0};
    qreal m_searchWidgetOpenProgress{0};
    qreal m_searchWidgetY{0};

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

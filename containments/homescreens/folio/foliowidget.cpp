// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "foliowidget.h"
#include "homescreenstate.h"
#include "widgetsmanager.h"

FolioWidget::FolioWidget(QObject *parent, int id, int gridWidth, int gridHeight)
    : QObject{parent}
    , m_id{id}
    , m_realGridWidth{gridWidth}
    , m_realGridHeight{gridHeight}
    , m_applet{nullptr}
    , m_quickApplet{nullptr}
{
    auto *applet = WidgetsManager::self()->getWidget(id);
    if (applet) {
        setApplet(applet);
    }
    init();
}

FolioWidget::FolioWidget(QObject *parent, Plasma::Applet *applet, int gridWidth, int gridHeight)
    : QObject{parent}
    , m_id{applet ? static_cast<int>(applet->id()) : -1}
    , m_realGridWidth{gridWidth}
    , m_realGridHeight{gridHeight}
{
    setApplet(applet);
    init();
}

void FolioWidget::init()
{
    connect(HomeScreenState::self(), &HomeScreenState::pageOrientationChanged, this, [this]() {
        Q_EMIT gridWidthChanged();
        Q_EMIT gridHeightChanged();
    });

    connect(WidgetsManager::self(), &WidgetsManager::widgetAdded, this, [this](Plasma::Applet *applet) {
        if (applet && static_cast<int>(applet->id()) == m_id) {
            setApplet(applet);
        }
    });
    connect(WidgetsManager::self(), &WidgetsManager::widgetRemoved, this, [this](Plasma::Applet *applet) {
        if (applet && static_cast<int>(applet->id()) == m_id) {
            setApplet(nullptr);
        }
    });
}

FolioWidget *FolioWidget::fromJson(QJsonObject &obj, QObject *parent)
{
    int id = obj[QStringLiteral("id")].toInt();
    int gridWidth = obj[QStringLiteral("gridWidth")].toInt();
    int gridHeight = obj[QStringLiteral("gridHeight")].toInt();
    return new FolioWidget(parent, id, gridWidth, gridHeight);
}

QJsonObject FolioWidget::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("type")] = "widget";
    obj[QStringLiteral("id")] = m_id;
    obj[QStringLiteral("gridWidth")] = m_realGridWidth;
    obj[QStringLiteral("gridHeight")] = m_realGridHeight;
    return obj;
}

int FolioWidget::id() const
{
    return m_id;
}

int FolioWidget::gridWidth() const
{
    switch (HomeScreenState::self()->pageOrientation()) {
    case HomeScreenState::RegularPosition:
        return m_realGridWidth;
    case HomeScreenState::RotateClockwise:
        return m_realGridHeight;
    case HomeScreenState::RotateCounterClockwise:
        return m_realGridHeight;
    case HomeScreenState::RotateUpsideDown:
        return m_realGridWidth;
    }
    return m_realGridWidth;
}

void FolioWidget::setGridWidth(int gridWidth)
{
    switch (HomeScreenState::self()->pageOrientation()) {
    case HomeScreenState::RegularPosition:
        setRealGridWidth(gridWidth);
        break;
    case HomeScreenState::RotateClockwise:
        setRealGridHeight(gridWidth);
        break;
    case HomeScreenState::RotateCounterClockwise:
        setRealGridHeight(gridWidth);
        break;
    case HomeScreenState::RotateUpsideDown:
        setRealGridWidth(gridWidth);
        break;
    }
}

int FolioWidget::gridHeight() const
{
    switch (HomeScreenState::self()->pageOrientation()) {
    case HomeScreenState::RegularPosition:
        return m_realGridHeight;
    case HomeScreenState::RotateClockwise:
        return m_realGridWidth;
    case HomeScreenState::RotateCounterClockwise:
        return m_realGridWidth;
    case HomeScreenState::RotateUpsideDown:
        return m_realGridHeight;
    }
    return m_realGridHeight;
}

void FolioWidget::setGridHeight(int gridHeight)
{
    switch (HomeScreenState::self()->pageOrientation()) {
    case HomeScreenState::RegularPosition:
        setRealGridHeight(gridHeight);
        break;
    case HomeScreenState::RotateClockwise:
        setRealGridWidth(gridHeight);
        break;
    case HomeScreenState::RotateCounterClockwise:
        setRealGridWidth(gridHeight);
        break;
    case HomeScreenState::RotateUpsideDown:
        setRealGridHeight(gridHeight);
        break;
    }
}

int FolioWidget::realGridWidth() const
{
    return m_realGridWidth;
}

void FolioWidget::setRealGridWidth(int gridWidth)
{
    if (m_realGridWidth != gridWidth) {
        m_realGridWidth = gridWidth;
        Q_EMIT gridWidthChanged();
    }
}

int FolioWidget::realGridHeight() const
{
    return m_realGridHeight;
}

void FolioWidget::setRealGridHeight(int gridHeight)
{
    if (m_realGridHeight != gridHeight) {
        m_realGridHeight = gridHeight;
        Q_EMIT gridHeightChanged();
    }
}

Plasma::Applet *FolioWidget::applet() const
{
    return m_applet;
}

void FolioWidget::setApplet(Plasma::Applet *applet)
{
    m_applet = applet;
    Q_EMIT appletChanged();

    if (m_applet) {
        setVisualApplet(PlasmaQuick::AppletQuickItem::itemForApplet(m_applet));
    } else {
        setVisualApplet(nullptr);
    }
}

PlasmaQuick::AppletQuickItem *FolioWidget::visualApplet() const
{
    return m_quickApplet;
}

void FolioWidget::setVisualApplet(PlasmaQuick::AppletQuickItem *quickItem)
{
    m_quickApplet = quickItem;
    Q_EMIT visualAppletChanged();
}

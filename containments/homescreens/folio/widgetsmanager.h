// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include <Plasma/Applet>

class WidgetsManager : public QObject
{
    Q_OBJECT
public:
    WidgetsManager(QObject *parent = nullptr);

    static WidgetsManager *self();

    Plasma::Applet *getWidget(int id);

    void addWidget(Plasma::Applet *applet);
    void removeWidget(Plasma::Applet *applet);

Q_SIGNALS:
    void widgetAdded(Plasma::Applet *applet);
    void widgetRemoved(Plasma::Applet *applet);

private:
    QList<Plasma::Applet *> m_widgets;
};

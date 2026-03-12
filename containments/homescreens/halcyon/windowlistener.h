// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QList>
#include <QObject>

class WindowListener : public QObject
{
    Q_OBJECT

public:
    WindowListener(QObject *parent = nullptr);

    static WindowListener *instance();

Q_SIGNALS:
    void windowChanged(QString storageId);
};

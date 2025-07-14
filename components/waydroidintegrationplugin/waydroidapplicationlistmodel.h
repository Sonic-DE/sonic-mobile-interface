/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "waydroidapplication.h"

#include <QAbstractListModel>
#include <QObject>

class WaydroidApplicationListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    WaydroidApplicationListModel(QObject *parent = nullptr);

private:
    QList<WaydroidApplication::Ptr> m_applications;

    void refreshApplications();
};
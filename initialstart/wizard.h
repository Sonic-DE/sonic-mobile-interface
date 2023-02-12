// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

class Wizard : public QObject
{
    Q_OBJECT

public:
    Wizard(QObject *parent = nullptr);

public Q_SLOTS:
    void wizardFinished();
};

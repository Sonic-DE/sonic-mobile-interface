// SPDX-FileCopyrightText: 2021 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <ModemManagerQt/Manager>

class SignalIndicator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int strength READ strength NOTIFY strengthChanged)

public:
    SignalIndicator();

    int strength() const;

Q_SIGNALS:
    void strengthChanged();

private:
    ModemManager::Modem::Ptr m_modem;
};

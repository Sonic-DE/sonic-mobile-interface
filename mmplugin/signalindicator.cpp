// SPDX-FileCopyrightText: 2021 Tobias Fella <fella@posteo.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "signalindicator.h"

SignalIndicator::SignalIndicator()
{
    m_modem = ModemManager::modemDevices()[0]->modemInterface();
    connect(m_modem.get(), &ModemManager::Modem::signalQualityChanged, this, &SignalIndicator::strengthChanged);
    connect(ModemManager::modemDevices()[0]->sim().get(), &ModemManager::Sim::operatorNameChanged, this, &SignalIndicator::nameChanged);
    connect(m_modem.get(), &ModemManager::Modem::unlockRequiredChanged, this, &SignalIndicator::simLockedChanged);
}

int SignalIndicator::strength() const
{
    return m_modem->signalQuality().signal;
}

QString SignalIndicator::name() const
{
    return ModemManager::modemDevices()[0]->sim()->operatorName();
}

bool SignalIndicator::simLocked() const
{
    return !(m_modem->unlockRequired() == MM_MODEM_LOCK_NONE || m_modem->unlockRequired() == MM_MODEM_LOCK_SIM_PIN2);
}
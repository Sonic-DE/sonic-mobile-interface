// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wizard.h"
#include "settings.h"

Wizard::Wizard(QObject *parent)
    : QObject{parent}
{
}

void Wizard::load()
{
}

void Wizard::wizardFinished()
{
    Settings::self()->setWizardFinished();
}

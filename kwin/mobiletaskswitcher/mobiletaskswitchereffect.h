// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "kwinquickeffect.h"

#include <QAction>
#include <QKeySequence>

#include <KGlobalAccel>
#include <KLocalizedString>

/**
 * Feature Scope:
 * -
 */

namespace KWin
{

class MobileTaskSwitcherEffect : public QuickSceneEffect
{
    Q_OBJECT

public:
    enum class Status { Inactive, Activating, Deactivating, Active };
    MobileTaskSwitcherEffect();
    ~MobileTaskSwitcherEffect() override;

    int requestedEffectChainPosition() const override;
    bool borderActivated(ElectricBorder border) override;
    void reconfigure(ReconfigureFlags flags) override;
    void grabbedKeyboardEvent(QKeyEvent *keyEvent) override;

public Q_SLOTS:
    void realDeactivate();
    void toggle();
    void activate();
    void deactivate();

private:
    QAction *m_toggleAction = nullptr;
    QList<QKeySequence> m_toggleShortcut;
    Status m_status = Status::Inactive;
};

} // namespace KWin

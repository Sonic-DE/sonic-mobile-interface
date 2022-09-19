// SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "mobiletaskswitchereffect.h"

namespace KWin
{

MobileTaskSwitcherEffect::MobileTaskSwitcherEffect()
{
    const QKeySequence defaultToggleShortcut = Qt::META | Qt::Key_T;
    m_toggleAction = new QAction(this);
    //     connect(m_toggleAction, &QAction::triggered, this, &MobileTaskSwitcherEffect::toggle);
    m_toggleAction->setObjectName(QStringLiteral("Task Switcher"));
    m_toggleAction->setText(i18n("Toggle Task Switcher"));
    KGlobalAccel::self()->setDefaultShortcut(m_toggleAction, {defaultToggleShortcut});
    KGlobalAccel::self()->setShortcut(m_toggleAction, {defaultToggleShortcut});
    effects->registerGlobalShortcut({defaultToggleShortcut}, m_toggleAction);

    //     connect(effects, &EffectsHandler::screenAboutToLock, this, &MobileTaskSwitcherEffect::realDeactivate);

    setSource(QUrl::fromLocalFile(
        QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwin/effects/mobiletaskswitcher/qml/TaskSwitcher.qml"))));
}

MobileTaskSwitcherEffect::~MobileTaskSwitcherEffect()
{
}

int MobileTaskSwitcherEffect::requestedEffectChainPosition() const
{
    return 0;
}

bool MobileTaskSwitcherEffect::borderActivated(ElectricBorder border)
{
    return false;
}

void MobileTaskSwitcherEffect::reconfigure(ReconfigureFlags flags)
{
}

void MobileTaskSwitcherEffect::grabbedKeyboardEvent(QKeyEvent *keyEvent)
{
}

void MobileTaskSwitcherEffect::activate()
{
    if (effects->isScreenLocked()) {
        return;
    }

    m_status = Status::Active;
    setRunning(true);
}

void MobileTaskSwitcherEffect::deactivate()
{
    //     const auto screenViews = views();
    //     for (QuickSceneView *view : screenViews) {
    //         QMetaObject::invokeMethod(view->rootItem(), "stop");
    //     }

    //     setRunning(false);
    //     m_status = Status::Inactive;
}

}

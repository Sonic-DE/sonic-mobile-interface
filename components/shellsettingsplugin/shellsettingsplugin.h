/*
 *  SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KConfigGroup>
#include <KConfigWatcher>
#include <KSharedConfig>
#include <QDBusConnection>
#include <QObject>

/**
 * @short Wrapper class to access and control mobile shell specific settings.
 *
 * @author Devin Lin <devin@kde.org>
 */
class ShellSettingsPlugin : public QObject
{
    Q_OBJECT
public:
    ShellSettingsPlugin(QObject *parent = nullptr);

private:
    void updateNavigationBarsInPlasma(bool navigationPanelEnabled);
};

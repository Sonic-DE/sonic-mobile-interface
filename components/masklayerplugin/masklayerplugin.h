// SPDX-FileCopyrightText: 2025 Micah Stnaley <stanleymicah@proton.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

class MaskLayerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT

public:
    void registerTypes(const char *uri) override;
};

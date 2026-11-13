// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KSharedConfig>

#include <QString>

namespace SessionTheme {
// Writes the light or dark global theme (by the session's "darkMode" setting) into the
// session's config overlay, locked so the desktop's own settings can't override it.
// Running apps are told when the theme changed.
void apply(const KSharedConfig::Ptr &sessionConfig, const QString &overlayDir, const QString &lightPackage, const QString &darkPackage);
}

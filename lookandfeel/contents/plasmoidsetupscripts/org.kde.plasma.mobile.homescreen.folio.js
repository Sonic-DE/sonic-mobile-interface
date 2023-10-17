// SPDX-FileCopyrightText: 2019-2020 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

applet.wallpaperPlugin = 'org.kde.image'
applet.writeConfig("Favourites", `[{"storageId": "org.kde.phone.dialer.desktop", "type": "application"}, {"storageId": "org.kde.spacebar.desktop", "type": "application"},{"storageId": "org.kde.angelfish.desktop", "type": "application"}]`);
applet.writeConfig("Pages", `[[{"column": 0, "row": 0, "storageId": "org.kde.qmlkonsole.desktop", "type": "application"}, {"column": 1, "row": 0, "storageId": "org.kde.kclock.desktop", "type": "application"}, {"column": 2, "row": 0, "storageId": "org.kde.kweather.desktop", "type": "application"}, {"column": 3, "row": 0, "storageId": "org.kde.krecorder.desktop", "type": "application"}]]`);
applet.reloadConfig()


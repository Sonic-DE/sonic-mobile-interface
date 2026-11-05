<!--
- SPDX-FileCopyrightText: None
- SPDX-License-Identifier: CC0-1.0
-->

# Plasma Mobile

This repository contains shell components for Plasma Mobile.

* Project page: [plasma-mobile.org](https://plasma-mobile.org)
* Repository: [invent.kde.org/plasma/plasma-mobile](https://invent.kde.org/plasma/plasma-mobile)
* Documentation: [invent.kde.org/plasma/plasma-mobile/-/wikis/home](https://invent.kde.org/plasma/plasma-mobile/-/wikis/home)
* Development channel: [matrix.to/#/#plasmamobile:matrix.org](https://matrix.to/#/#plasmamobile:matrix.org)

### Reporting issues
* How to report issues: [invent.kde.org/plasma/plasma-mobile/-/wikis/Issue-Tracking](https://invent.kde.org/plasma/plasma-mobile/-/wikis/Issue-Tracking)
* Shell issue tracker: [invent.kde.org/plasma/plasma-mobile/-/issues](https://invent.kde.org/plasma/plasma-mobile/-/issues)
* General issue tracker: [https://invent.kde.org/teams/plasma-mobile/issues/-/issues](https://invent.kde.org/teams/plasma-mobile/issues/-/issues)

### Locations
* [components/mobileshell](components/mobileshell) - private shell component library (API not guaranteed to be stable!)
* [containments](containments) - shell panels (homescreens, status bar, task panel)
* [kcms](kcms) - settings module
* [look-and-feel](look-and-feel/contents) - Plasma look-and-feel packages (ex. lockscreen, logout, etc.)
* [shell](shell) - Plasma shell package, provides implementations for applet and containment configuration dialogs
* [quicksettings](quicksettings) - quick settings packages for the action drawer
* [tests](tests) - small runnable snippets that can be used to test parts of the shell without loading all of Plasma

<img src="/screenshots/homescreen-folio.png" width=300px/>
<img src="/screenshots/homescreen-halcyon.png" width=300px/>

### Test on a development machine

See the [documentation page](https://invent.kde.org/plasma/plasma-mobile/-/wikis/Building-and-Testing-Locally) for more details.

It is recommended to use `kdesrc-build` to build this from source. See [this page](https://community.kde.org/Get_Involved/development) in order to set it up.

Dependencies:
* KDE Frameworks 6 setup (plasma-framework and its dependencies)
* plasma-workspace
* plasma-nm
* plasma-pa
* bluez-qt
* Milou (for search)
* Kirigami
* Kirigami Addons
* sonic-keyboard
* feedbackd (optional: for vibrations)

The shell runs on X11. To start it on a nested or virtual X server, with `sonic-win` as its window manager, run:

```
QT_QUICK_CONTROLS_MOBILE=true PLASMA_PLATFORM=phone:handset dbus-run-session sonicshell -p org.kde.plasma.mobileshell --no-respawn
```

Use your X server's own options to set the screen size, such as `Xephyr -screen 360x720`.

### Device specific configuration

See [/devices/README.md](/devices/README.md) for more details on setting device specific configuration (ex. notches, screen curves).

---

<img src="https://invent.kde.org/plasma/plasma-mobile/-/wikis/uploads/19a607bb68faa76bbc9f888e33a3aa9a/konqi-calling.png" width=200px>

<br/>

<img src="https://invent.kde.org/plasma/plasma-mobile/-/wikis/uploads/9238173a7cae1d8832d83350eda74f85/developers.png" width=300px>

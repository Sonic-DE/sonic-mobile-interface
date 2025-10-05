This folder contains source files for implementing the configuration window for applets/containments (including homescreens).

[libplasma](https://invent.kde.org/frameworks/libplasma) loads either `AppletConfiguration.qml` or `ContainmentConfiguration.qml` (depending on if its an applet or containment) from this folder when requested by the shell, which in turn initializes the config window.

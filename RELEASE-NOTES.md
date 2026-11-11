# Sonic Mobile Interface Release Notes

SonicDE's touch-friendly shell for phones and tablets.

## Unreleased

- The **Record Screen** quick setting is back. It records the screen to your Videos folder and tells you where the video was saved.
- The mobile session keeps your own global theme and splash screen, with Silver as the default, instead of switching them to its own Breeze-based theme.
- Behind the scenes: removed the Waydroid leftovers (translations, hidden app list entries, and a "Waydroid Management" label that the flashlight's permission prompt carried by mistake) until SonicDE has its own Android app support.

## 6.8.2 (2026-11-10)

- The mobile shell now runs on X11, where it used to show only a black screen, and starts with the SonicDE on-screen keyboard. Navigation uses panel buttons until gestures work on X11, and Waydroid support and the screen recording quick setting have been removed.
- Now presents itself with SonicDE names and identity throughout the desktop.
- The mobile session is now called "SonicDE Mobile" on the login screen, in every language SonicDE is translated into. If you used it before, choose it once more after upgrading.
- Behind the scenes: new automated tests for the X11 shell, and repository tidying.

## 6.8.0 (2026-10-20)

- Behind the scenes: updated to build against SonicDE 6.8.0 and a version number update.

## 6.7.5 (2026-09-12)

- Fewer legacy compatibility packages are needed: the mobile shell no longer depends on the old Plasma and Qt 5 support libraries.
- Behind the scenes: version number update.

## 6.7.4.2 (2026-08-20)

- The Silver wallpaper is now the default in the remaining places that still used the old one.

## 6.7.4.1 (2026-08-12)

- The Silver wallpaper is now the default background.

## 6.7.4 (2026-08-04)

No SonicDE-specific changes in this release; it brings the package up to date with upstream.

## 6.7.3.2 (2026-08-02)

- The KDE logo has been replaced with the SonicDE logo throughout.

## 6.7.3.1 (2026-08-01)

- SonicDE's own Silver theme replaces Breeze as the base and default look.
- Behind the scenes: shared code formatting settings.

## 6.7.3 (2026-07-14)

- Behind the scenes: code formatting update.

## 6.7.1 (2026-06-23)

- Behind the scenes: developer tooling update.

## 6.7.0 (2026-06-18)

No SonicDE-specific changes in this release; it brings the package up to date with upstream.

## 6.6.5 (2026-05-12)

- Streamlined for SonicDE's X11 desktop: Wayland dependencies have been removed from the mobile shell.
- Behind the scenes: build fixes for SonicDE's window manager and other components, adjustments to the setup wizard's screen detection and task switcher after the Wayland removal, developer tooling support and code clean-ups.

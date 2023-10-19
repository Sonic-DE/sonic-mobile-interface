<!--
- SPDX-FileCopyrightText: None
- SPDX-License-Identifier: CC0-1.0
-->

# Folio Homescreen

This is the paged homescreen for Plasma Mobile.

### How it works

Most of the homescreen is in C++ in order to keep logic together, with QML only responsible for the display and user input.

As such, all of the positioning and placement of delegates on the screen are top down from the model, as well as drag and drop behaviour.

#### TODO
- If an app gets uninstalled, the homescreen UI needs to ensure that delegates are updated
- Folder pages
- Fix drawer scrolling
- the position of where things think the dragged icon is during drag-and-drop is slightly off because of the label
- BUG: landscape favourites bar duplication when dragging icon from it
- BUG: dropping an app on the favourites bar animation is glitched ONLY if the nav bar is on the bottom

move drop animation code to c++

- can make the touch area only the icon?

- BUG: from default config, the first icon disappears when dragging from the app drawer

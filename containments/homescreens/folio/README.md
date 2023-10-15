# Folio Homescreen

This is the paged homescreen for Plasma Mobile.

### How it works

Most of the homescreen is in C++ in order to keep logic together, with QML only responsible for the display and user input.

As such, all of the positioning and placement of delegates on the screen are top down from the model, as well as drag and drop behaviour.

#### TODO
- If an app gets uninstalled, the homescreen UI needs to ensure that delegates are updated
- Folder pages
- Fix drawer scrolling
- Drop animation (delegate moves to spot)

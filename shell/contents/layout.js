var desktopsArray = desktopsForActivity(currentActivity());
for (var j = 0; j < desktopsArray.length; j++) {
    desktopsArray[j].wallpaperPlugin = "org.kde.image";
}
desktopsArray[0].addWidget("org.kde.phone.krunner", 0, 0, screenGeometry(0).width, 20)


// load top panel

// list of applets that will be loaded in the top panel
// keep this list in sync with shell/contents/updates/panelsfix.js
var panel = new Panel("org.kde.phone.panel");
panel.addWidget("org.kde.plasma.notifications");
panel.addWidget("org.kde.plasma.mediacontroller");
panel.addWidget("org.kde.plasma.volume");
panel.addWidget("org.kde.plasma.bluetooth");
panel.addWidget("org.kde.plasma.networkmanagement");
panel.height = 1 * gridUnit;


// load bottom panel

var bottomPanel = new Panel("org.kde.phone.taskpanel");
bottomPanel.location = "bottom";
if (screenGeometry(bottomPanel.screen).height > screenGeometry(bottomPanel.screen).width) {
    bottomPanel.height = 2 * gridUnit;
} else {
    bottomPanel.height = 1 * gridUnit;
}

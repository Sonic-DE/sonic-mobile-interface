
function dbg(msg) {
    console.debug("kwin_scripting::js " + msg);
}

function analyze_window(w) {
    dbg("___ Caption: " + w.caption + "______________");
    dbg("       windowRole: " + w.windowRole);
    dbg("     normalWindow: " + w.normalWindow);
    dbg("           dialog: " + w.dialog);
    dbg("          utility: " + w.utility);
    dbg("    specialWindow: " + w.specialWindow);
    dbg("            modal: " + w.modal);
    dbg("____________________________________________");
}

function fake_place(sss) {
    //analyze_window(window);
    //let area = Qt.rect(10, 20, 30, 40);
    //const area = { x: 100; y: 200; width: 500; height: 600 }
    dbg("... place() got string" + sss);
    dbg("rect 10 20 30 40");
    return "greetings from my script!";
    //return area;
}

function init() {
    const windows = workspace.windowList();

    dbg("I found " + windows.length + " windows. :)");
    for (var i = 0; i < windows.length; i++) {
        var window = windows[i];
        print("init: " + window.caption);

        const output = window.output;
        const desktop = window.desktops[0]; // assume it's the first desktop that the window is on
        analyze_window(window);
        if (desktop === undefined) {
            dbg("desktop undefined.");
            return;
        }
        const maximizeRect = workspace.clientArea(workspace.MaximizeArea, output, desktop);
        analyze_window(window);
        place(window, maximizeRect);
    }
}

function place(window, clientArea) {
    dbg("Hey there. installed script is running!");
    analyze_window(window);
    //dbg("clientArea in script: " + clientArea);
    //let area = Qt.rect(10, 20, 30, 40);
    dbg("Available clientArea: " + clientArea.x + " " + clientArea.y + " " + clientArea.width + " " + clientArea.height);
    var empty_rect = {};

    if (!window) {
        dbg("Window pointer empty")
        return empty_rect;
    }

    if (!window || window.resourceClass === 'xwaylandvideobridge') {
        return empty_rect;
    }

    if (!window.modal) {
        dbg("non modal, maximizing.");

        var w_ = clientArea.width;
        var h_ = clientArea.height;

        //window.frameGeometry = {x: 0, y: 0, width: w_, height: h_ }
        window.setMaximize(true, true);
        window.noBorder = true;


        var w_width = clientArea.width / 3;
        var w_height = clientArea.height / 3;
        return {x: 0, y: 0, width: w_width, height: w_height }; // FIXME: x and y.
    } else {
        dbg("modal, trying to ignore.")
        return empty_rect;
    }
    // var area = { x: 100, y: 200, width: 500, height: 600 };
    // dbg("place() will return a rect 100:200:500:600");
    // dbg("rect 10 20 30 40");
    // return area;
    //return "Blergh";
}




dbg("installed script mobilewindows is running.");

workspace.setPlacementCallback(place);

dbg("running init().");

init();

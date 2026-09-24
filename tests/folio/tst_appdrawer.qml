// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtTest
import org.kde.kirigami as Kirigami
import plasma.applet.org.kde.plasma.mobile.homescreen.folio as Folio
import "../../containments/homescreens/folio/qml" as Drawer

TestCase {
    id: testCase
    name: "FolioAppDrawer"
    when: windowShown
    visible: true
    width: 480
    height: 800

    Component {
        id: headerComponent
        Drawer.AppDrawerHeader {
            folio: testFolio
            width: 480
            height: implicitHeight
        }
    }

    Component {
        id: touchAreaComponent
        Folio.DelegateTouchArea { width: 100; height: 100 }
    }

    Component {
        id: searchComponent
        Folio.ApplicationListSearchModel { sourceModel: testFolio.ApplicationListModel }
    }

    SignalSpy { id: focusSpy; signalName: "focusGridRequested" }
    SignalSpy { id: clickSpy; signalName: "clicked" }

    function cleanup() {
        focusSpy.target = null;
        clickSpy.target = null;
        testFolio.HomeScreenState.goToAppDrawerPage(0, true);
    }

    function test_allAppsPreservesInstalledApplications() {
        const model = testFolio.ApplicationListModel;
        model.sycocaDbChanged();
        let actual = [];
        for (let row = 0; row < model.rowCount(); ++row) {
            actual.push(model.data(model.index(row, 0), Qt.DisplayRole).application.storageId);
        }
        compare(actual.sort(), Array.from(expectedStorageIds).sort());
    }

    function test_searchAndCategoryFilters() {
        const search = createTemporaryObject(searchComponent, testCase);
        verify(search);
        compare(search.count, expectedStorageIds.length);
        search.categoryFilter = "nonexistent-test-category";
        compare(search.count, 0);
        search.categoryFilter = "";
        compare(search.count, expectedStorageIds.length);
        search.searchString = "nonexistent-test-application-123456";
        compare(search.count, 0);
        search.searchString = "";
        compare(search.count, expectedStorageIds.length);
    }

    function test_lookupDoesNotTransferDelegateOwnership() {
        const search = createTemporaryObject(searchComponent, testCase);
        verify(search);
        if (search.count === 0) {
            skip("No installed applications available for the ownership test");
        }
        const storageId = search.get(0, "delegate").application.storageId;
        gc();
        wait(0);
        compare(search.get(0, "delegate").application.storageId, storageId);
    }

    function test_headerKeyboardNavigation_data() {
        return [
            { tag: "down", key: Qt.Key_Down },
            { tag: "tab", key: Qt.Key_Tab }
        ];
    }

    function test_headerKeyboardNavigation(data) {
        const header = createTemporaryObject(headerComponent, testCase);
        verify(header);
        focusSpy.target = header;
        focusSpy.clear();
        header.focusSearchBar();
        verify(header.searchField.activeFocus);

        keyClick(data.key);
        verify(header.tabbar.activeFocus);
        verify(header.tabbar.keyboardFocus);
        compare(focusSpy.count, 0);

        keyClick(data.key);
        compare(focusSpy.count, 1);
    }

    function test_headerReverseKeyboardNavigation_data() {
        return [
            { tag: "up", key: Qt.Key_Up },
            { tag: "backtab", key: Qt.Key_Backtab }
        ];
    }

    function test_headerReverseKeyboardNavigation(data) {
        const header = createTemporaryObject(headerComponent, testCase);
        verify(header);
        header.focusTabBar();
        verify(header.tabbar.activeFocus);
        keyClick(data.key);
        verify(header.searchField.activeFocus);
        compare(header.tabbar.keyboardFocus, false);
    }

    function test_headerPillUsesCompactSizing() {
        const header = createTemporaryObject(headerComponent, testCase);
        verify(header);
        compare(header.tabbar.implicitHeight, Kirigami.Units.gridUnit * 1.5);
        compare(header.tabbar.font.pointSize, Kirigami.Theme.smallFont.pointSize);
        compare(header.tabbar.font.pixelSize, Kirigami.Theme.smallFont.pixelSize);
    }

    function test_forwardedSearchReturnsToAllApps() {
        const header = createTemporaryObject(headerComponent, testCase);
        verify(header);
        testFolio.HomeScreenState.appDrawerPageCount = 2;
        testFolio.HomeScreenState.goToAppDrawerPage(1, true);
        compare(testFolio.HomeScreenState.currentAppDrawerPage, 1);
        header.addSearchText("test");
        compare(testFolio.HomeScreenState.currentAppDrawerPage, 0);
    }

    function test_noninteractiveDelegatesDoNotReceiveClicks() {
        const area = createTemporaryObject(touchAreaComponent, testCase);
        verify(area);
        clickSpy.target = area;
        clickSpy.clear();
        mouseClick(area, 50, 50);
        compare(clickSpy.count, 1);
        area.interactive = false;
        mouseClick(area, 50, 50);
        compare(clickSpy.count, 1);
    }
}

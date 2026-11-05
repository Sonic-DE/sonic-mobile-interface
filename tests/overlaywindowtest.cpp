// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "overlaywindow.h"
#include "shellutil.h"

#include <KWindowInfo>
#include <NETWM>

#include <QGuiApplication>
#include <QTest>

#include <xcb/shape.h>
#include <xcb/xcb.h>

class OverlayWindowTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        if (!qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
            QSKIP("Needs an X server");
        }
    }

    void testType_data()
    {
        QTest::addColumn<OverlayWindow::Type>("type");
        QTest::addColumn<NET::WindowType>("expected");
        QTest::newRow("dock") << OverlayWindow::Dock << NET::Dock;
        QTest::newRow("notification") << OverlayWindow::Notification << NET::Notification;
        QTest::newRow("osd") << OverlayWindow::OnScreenDisplay << NET::OnScreenDisplay;
    }

    void testType()
    {
        QFETCH(OverlayWindow::Type, type);
        QFETCH(NET::WindowType, expected);

        OverlayWindow window;
        window.setType(type);
        window.resize(100, 100);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));

        // Showing and changing flags must not put back the type Qt derives from the flags.
        ShellUtil().setInputTransparent(&window, true);
        ShellUtil().setInputTransparent(&window, false);
        qApp->processEvents();

        const KWindowInfo info(window.winId(), NET::WMWindowType);
        QCOMPARE(info.windowType(NET::AllTypesMask), expected);
    }

    void testFocusFollowsTakeFocus()
    {
        OverlayWindow window;
        QVERIFY(window.flags() & Qt::WindowDoesNotAcceptFocus);

        window.setTakeFocus(true);
        QVERIFY(!(window.flags() & Qt::WindowDoesNotAcceptFocus));

        window.setTakeFocus(false);
        QVERIFY(window.flags() & Qt::WindowDoesNotAcceptFocus);
    }

    void testInputRegion()
    {
        OverlayWindow window;
        window.resize(200, 100);
        window.show();
        QVERIFY(QTest::qWaitForWindowExposed(&window));
        ShellUtil util;
        const qreal scale = window.devicePixelRatio();
        const auto scaled = [scale](int value) {
            return qRound(value * scale);
        };

        util.setInputRegion(&window, QRect(10, 20, 30, 40));
        QCOMPARE(inputRectangles(window), QList<QRect>{QRect(scaled(10), scaled(20), scaled(30), scaled(40))});

        // Transparency wins while it is on, and the region comes back afterwards.
        util.setInputTransparent(&window, true);
        QVERIFY(inputRectangles(window).isEmpty());
        util.setInputRegion(&window, QRect(1, 2, 3, 4));
        QVERIFY(inputRectangles(window).isEmpty());
        util.setInputTransparent(&window, false);
        QCOMPARE(inputRectangles(window), QList<QRect>{QRect(scaled(1), scaled(2), scaled(3), scaled(4))});

        // An empty region means the whole window.
        util.setInputRegion(&window, QRect());
        QCOMPARE(inputRectangles(window), QList<QRect>{QRect(0, 0, scaled(200), scaled(100))});
    }

private:
    static QList<QRect> inputRectangles(const QWindow &window)
    {
        xcb_connection_t *connection = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()->connection();
        qApp->processEvents();

        const auto cookie = xcb_shape_get_rectangles(connection, window.winId(), XCB_SHAPE_SK_INPUT);
        std::unique_ptr<xcb_shape_get_rectangles_reply_t, decltype(&free)> reply(xcb_shape_get_rectangles_reply(connection, cookie, nullptr), &free);
        if (!reply) {
            return {};
        }

        QList<QRect> result;
        const xcb_rectangle_t *rectangles = xcb_shape_get_rectangles_rectangles(reply.get());
        const int count = xcb_shape_get_rectangles_rectangles_length(reply.get());
        for (int i = 0; i < count; ++i) {
            if (rectangles[i].width && rectangles[i].height) {
                result.append(QRect(rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height));
            }
        }
        return result;
    }
};

QTEST_MAIN(OverlayWindowTest)

#include "overlaywindowtest.moc"

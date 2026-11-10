// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

// Records a private Xvfb screen showing a known colour, then decodes the file and checks it.

#include "x11screenrecorder.h"

#include <QFile>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QProcess>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>
#include <QVideoSink>

#include <Sonic/Logging/Logging>

#include <fcntl.h>
#include <unistd.h>

SONIC_LOGGING_COMPONENT("org.sonicde.mobile.screenrecorder.test")

using namespace Qt::StringLiterals;

namespace {
bool closeTo(QRgb actual, QRgb expected, int tolerance)
{
    return std::abs(qRed(actual) - qRed(expected)) <= tolerance && std::abs(qGreen(actual) - qGreen(expected)) <= tolerance
        && std::abs(qBlue(actual) - qBlue(expected)) <= tolerance;
}
}

class ScreenRecorderTest : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void convertsColours_data();
    void convertsColours();
    void recordsScreen();

private:
    QProcess m_xvfb;
};

void ScreenRecorderTest::initTestCase()
{
    int fds[2];
    QVERIFY(::pipe(fds) == 0);
    m_xvfb.setChildProcessModifier([readEnd = fds[0]] {
        ::close(readEnd);
    });
    m_xvfb.start(QStringLiteral(XVFB_PATH), {u"-displayfd"_s, QString::number(fds[1]), u"-screen"_s, u"0"_s, u"320x240x24"_s, u"-nolisten"_s, u"tcp"_s});
    ::close(fds[1]);
    QVERIFY2(m_xvfb.waitForStarted(), qPrintable(m_xvfb.errorString()));

    QFile displayFd;
    QVERIFY(displayFd.open(fds[0], QIODevice::ReadOnly, QFileDevice::AutoCloseHandle));
    QByteArray display;
    for (int i = 0; i < 200 && !display.contains('\n'); ++i) {
        display += displayFd.read(16);
        if (!display.contains('\n')) {
            ::usleep(50000);
        }
    }
    QVERIFY2(display.contains('\n'), "Xvfb did not report its display");
    qputenv("DISPLAY", ':' + display.trimmed());
}

void ScreenRecorderTest::cleanupTestCase()
{
    m_xvfb.terminate();
    m_xvfb.waitForFinished();
}

void ScreenRecorderTest::convertsColours_data()
{
    QTest::addColumn<QRgb>("colour");
    QTest::newRow("red") << qRgb(255, 0, 0);
    QTest::newRow("green") << qRgb(0, 255, 0);
    QTest::newRow("blue") << qRgb(0, 0, 255);
    QTest::newRow("white") << qRgb(255, 255, 255);
    QTest::newRow("black") << qRgb(0, 0, 0);
    QTest::newRow("grey") << qRgb(128, 128, 128);
}

void ScreenRecorderTest::convertsColours()
{
    QFETCH(QRgb, colour);
    QImage image(16, 8, QImage::Format_RGB32);
    image.fill(colour);
    QVideoFrame frame = X11ScreenRecorder::toYuv420Frame(image);
    QVERIFY(frame.isValid());
    QCOMPARE(frame.pixelFormat(), QVideoFrameFormat::Format_YUV420P);
    const QImage back = frame.toImage();
    QCOMPARE(back.size(), image.size());
    const QRgb actual = back.pixel(5, 3);
    QVERIFY2(closeTo(actual, colour, 6), qPrintable(u"got %1 for %2"_s.arg(actual, 8, 16).arg(colour, 8, 16)));
}

void ScreenRecorderTest::recordsScreen()
{
    const std::optional<X11ScreenRecorder::Format> format = X11ScreenRecorder::availableFormat();
    QVERIFY2(format, "no VP9 or H.264 encoder available");

    // A window covering the screen in a known colour.
    xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
    QVERIFY(!xcb_connection_has_error(connection));
    const xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    const QSize screenSize(screen->width_in_pixels, screen->height_in_pixels);
    const xcb_window_t window = xcb_generate_id(connection);
    const uint32_t values[] = {0x00ff0000, 1};
    xcb_create_window(connection,
                      XCB_COPY_FROM_PARENT,
                      window,
                      screen->root,
                      0,
                      0,
                      screen->width_in_pixels,
                      screen->height_in_pixels,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT,
                      values);
    xcb_map_window(connection, window);
    std::free(xcb_get_input_focus_reply(connection, xcb_get_input_focus(connection), nullptr));

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.filePath(u"recording."_s + format->suffix);
    X11ScreenRecorder recorder;
    QSignalSpy finished(&recorder, &X11ScreenRecorder::finished);
    QSignalSpy failed(&recorder, &X11ScreenRecorder::failed);
    QVERIFY2(recorder.start(file, QRect(QPoint(0, 0), screenSize), *format), qPrintable(recorder.error()));
    QTimer::singleShot(1500, &recorder, &X11ScreenRecorder::finish);
    QVERIFY2(finished.wait(20000), qPrintable(recorder.error()));
    QCOMPARE(failed.count(), 0);
    xcb_disconnect(connection);
    QVERIFY(QFileInfo(file).size() > 0);

    QMediaPlayer player;
    QVideoSink sink;
    player.setVideoSink(&sink);
    QImage decoded;
    connect(&sink, &QVideoSink::videoFrameChanged, this, [&](const QVideoFrame &frame) {
        if (decoded.isNull() && frame.isValid()) {
            decoded = frame.toImage();
        }
    });
    player.setSource(QUrl::fromLocalFile(file));
    player.play();
    QTRY_VERIFY_WITH_TIMEOUT(!decoded.isNull() || player.error() != QMediaPlayer::NoError, 10000);
    QCOMPARE(player.error(), QMediaPlayer::NoError);
    QVERIFY(player.duration() >= 1000);
    QCOMPARE(decoded.size(), screenSize);
    const QRgb centre = decoded.pixel(decoded.width() / 2, decoded.height() / 2);
    QVERIFY2(closeTo(centre, qRgb(255, 0, 0), 24), qPrintable(u"centre pixel %1"_s.arg(centre, 8, 16)));
}

QTEST_MAIN(ScreenRecorderTest)

#include "screenrecordertest.moc"

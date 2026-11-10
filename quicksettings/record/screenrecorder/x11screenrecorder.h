// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QImage>
#include <QMediaFormat>
#include <QObject>
#include <QRect>
#include <QTimer>
#include <QVideoFrame>

#include <memory>
#include <optional>

#include <xcb/xcb.h>

class QMediaCaptureSession;
class QMediaRecorder;
class QVideoFrameInput;

// Records an area of the X11 screen to a video file: frames come from the X server
// (xcb_get_image on the root window) and Qt Multimedia encodes them.
class X11ScreenRecorder : public QObject {
    Q_OBJECT

public:
    struct Format {
        QMediaFormat::FileFormat fileFormat;
        QMediaFormat::VideoCodec codec;
        QString suffix;
    };

    explicit X11ScreenRecorder(QObject *parent = nullptr);
    ~X11ScreenRecorder() override;

    // The first file format and codec this system can encode, VP9 in WebM before H.264 in MP4.
    static std::optional<Format> availableFormat();

    // Starts recording area of the screen (in X11 pixels) into file; false with error() set on failure.
    bool start(const QString &file, const QRect &area, const Format &format, int fps = 30);
    // Stops grabbing and writes the rest of the file; finished() or failed() follows.
    void finish();

    QString error() const;

    // An RGB32 image as a YUV 4:2:0 frame (BT.601), the format the encoder input takes.
    static QVideoFrame toYuv420Frame(const QImage &image);

Q_SIGNALS:
    void finished();
    void failed(const QString &message);

private:
    void grabFrame();
    void fail(const QString &message);

    xcb_connection_t *m_connection = nullptr;
    xcb_window_t m_root = XCB_NONE;
    QRect m_area;
    int m_frameUs = 33333;
    QTimer m_timer;
    QElapsedTimer m_clock;
    bool m_inputReady = false;
    bool m_finishing = false;
    bool m_done = false;
    std::unique_ptr<QMediaCaptureSession> m_session;
    std::unique_ptr<QMediaRecorder> m_recorder;
    std::unique_ptr<QVideoFrameInput> m_input;
    QString m_error;
};

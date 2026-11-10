// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "x11screenrecorder.h"

#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QUrl>
#include <QVideoFrameFormat>
#include <QVideoFrameInput>

#include <Sonic/Logging/Log>

#include <algorithm>
#include <cstdlib>

using namespace Qt::StringLiterals;

X11ScreenRecorder::X11ScreenRecorder(QObject *parent)
    : QObject(parent)
{
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &X11ScreenRecorder::grabFrame);
}

X11ScreenRecorder::~X11ScreenRecorder()
{
    m_timer.stop();
    if (m_session) {
        m_session->setVideoFrameInput(nullptr);
        m_session->setRecorder(nullptr);
    }
    m_input.reset();
    m_recorder.reset();
    m_session.reset();
    if (m_connection) {
        xcb_disconnect(m_connection);
    }
}

std::optional<X11ScreenRecorder::Format> X11ScreenRecorder::availableFormat()
{
    const Format candidates[] = {
        {QMediaFormat::WebM, QMediaFormat::VideoCodec::VP9, u"webm"_s},
        {QMediaFormat::MPEG4, QMediaFormat::VideoCodec::H264, u"mp4"_s},
    };
    for (const Format &candidate : candidates) {
        QMediaFormat probe;
        if (!probe.supportedFileFormats(QMediaFormat::Encode).contains(candidate.fileFormat)) {
            continue;
        }
        probe.setFileFormat(candidate.fileFormat);
        if (probe.supportedVideoCodecs(QMediaFormat::Encode).contains(candidate.codec)) {
            return candidate;
        }
    }
    return std::nullopt;
}

bool X11ScreenRecorder::start(const QString &file, const QRect &area, const Format &format, int fps)
{
    // 4:2:0 frames need even sizes.
    m_area = QRect(area.topLeft(), QSize(area.width() & ~1, area.height() & ~1));
    if (m_area.isEmpty()) {
        m_error = u"The area to record is empty"_s;
        return false;
    }
    m_connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(m_connection)) {
        m_error = u"Could not connect to the X11 display"_s;
        return false;
    }
    m_root = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data->root;

    QVideoFrameFormat frameFormat(m_area.size(), QVideoFrameFormat::Format_YUV420P);
    frameFormat.setStreamFrameRate(fps);
    m_session = std::make_unique<QMediaCaptureSession>();
    m_recorder = std::make_unique<QMediaRecorder>();
    m_input = std::make_unique<QVideoFrameInput>(frameFormat);
    QMediaFormat mediaFormat;
    mediaFormat.setFileFormat(format.fileFormat);
    mediaFormat.setVideoCodec(format.codec);
    m_recorder->setMediaFormat(mediaFormat);
    m_recorder->setOutputLocation(QUrl::fromLocalFile(file));
    m_recorder->setVideoResolution(m_area.size());
    m_recorder->setVideoFrameRate(fps);
    m_recorder->setAutoStop(true);
    m_session->setRecorder(m_recorder.get());
    m_session->setVideoFrameInput(m_input.get());

    connect(m_input.get(), &QVideoFrameInput::readyToSendVideoFrame, this, [this] {
        m_inputReady = true;
        if (m_finishing && !m_done) {
            // The end of the stream; autoStop then stops the recorder.
            m_inputReady = !m_input->sendVideoFrame({});
        }
    });
    connect(m_recorder.get(), &QMediaRecorder::recorderStateChanged, this, [this](QMediaRecorder::RecorderState state) {
        if (state == QMediaRecorder::StoppedState && m_finishing && !m_done) {
            m_done = true;
            Q_EMIT finished();
        }
    });
    connect(m_recorder.get(), &QMediaRecorder::errorOccurred, this, [this](QMediaRecorder::Error, const QString &message) {
        fail(message.isEmpty() ? u"The video encoder failed"_s : message);
    });

    m_inputReady = true;
    m_recorder->record();
    if (m_recorder->error() != QMediaRecorder::NoError) {
        m_error = m_recorder->errorString();
        return false;
    }
    m_frameUs = 1000000 / std::max(1, fps);
    m_clock.start();
    m_timer.start(std::max(1, 1000 / std::max(1, fps)));
    sonicInfo() << "recording" << m_area << "to" << file;
    return true;
}

void X11ScreenRecorder::finish()
{
    if (m_finishing || m_done || !m_recorder) {
        return;
    }
    m_finishing = true;
    m_timer.stop();
    if (m_inputReady) {
        m_inputReady = !m_input->sendVideoFrame({});
    }
    if (m_recorder->recorderState() == QMediaRecorder::StoppedState) {
        m_done = true;
        Q_EMIT finished();
    }
}

QString X11ScreenRecorder::error() const
{
    return m_error;
}

void X11ScreenRecorder::grabFrame()
{
    // The encoder's queue is full: drop this tick rather than block.
    if (!m_inputReady || m_finishing) {
        return;
    }
    const qint64 startUs = m_clock.nsecsElapsed() / 1000;
    const auto cookie = xcb_get_image(m_connection, XCB_IMAGE_FORMAT_Z_PIXMAP, m_root, m_area.x(), m_area.y(), m_area.width(), m_area.height(), ~0u);
    xcb_get_image_reply_t *reply = xcb_get_image_reply(m_connection, cookie, nullptr);
    if (!reply) {
        fail(u"The X server returned no image"_s);
        return;
    }
    const int length = xcb_get_image_data_length(reply);
    const int stride = m_area.height() > 0 ? length / m_area.height() : 0;
    if (reply->depth < 24 || stride < m_area.width() * 4) {
        const int depth = reply->depth;
        std::free(reply);
        fail(u"The screen's pixel format isn't supported (depth %1)"_s.arg(depth));
        return;
    }
    const QImage image(xcb_get_image_data(reply), m_area.width(), m_area.height(), stride, QImage::Format_RGB32);
    QVideoFrame frame = toYuv420Frame(image);
    std::free(reply);
    if (!frame.isValid()) {
        fail(u"Could not prepare a video frame"_s);
        return;
    }
    frame.setStartTime(startUs);
    frame.setEndTime(startUs + m_frameUs);
    m_inputReady = m_input->sendVideoFrame(frame);
}

QVideoFrame X11ScreenRecorder::toYuv420Frame(const QImage &source)
{
    const QImage image = source.format() == QImage::Format_RGB32 ? source : source.convertToFormat(QImage::Format_RGB32);
    QVideoFrame frame(QVideoFrameFormat(image.size(), QVideoFrameFormat::Format_YUV420P));
    if (!frame.map(QVideoFrame::WriteOnly)) {
        return {};
    }
    const auto byte = [](int value) {
        return static_cast<uchar>(std::clamp(value, 0, 255));
    };
    for (int y = 0; y < image.height(); ++y) {
        uchar *luma = frame.bits(0) + y * frame.bytesPerLine(0);
        const auto *pixels = reinterpret_cast<const QRgb *>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            luma[x] = byte(((66 * qRed(pixels[x]) + 129 * qGreen(pixels[x]) + 25 * qBlue(pixels[x]) + 128) >> 8) + 16);
        }
    }
    for (int y = 0; y + 1 < image.height(); y += 2) {
        uchar *u = frame.bits(1) + (y / 2) * frame.bytesPerLine(1);
        uchar *v = frame.bits(2) + (y / 2) * frame.bytesPerLine(2);
        const auto *top = reinterpret_cast<const QRgb *>(image.constScanLine(y));
        const auto *bottom = reinterpret_cast<const QRgb *>(image.constScanLine(y + 1));
        for (int x = 0; x + 1 < image.width(); x += 2) {
            const int r = (qRed(top[x]) + qRed(top[x + 1]) + qRed(bottom[x]) + qRed(bottom[x + 1])) / 4;
            const int g = (qGreen(top[x]) + qGreen(top[x + 1]) + qGreen(bottom[x]) + qGreen(bottom[x + 1])) / 4;
            const int b = (qBlue(top[x]) + qBlue(top[x + 1]) + qBlue(bottom[x]) + qBlue(bottom[x + 1])) / 4;
            u[x / 2] = byte(((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128);
            v[x / 2] = byte(((112 * r - 94 * g - 18 * b + 128) >> 8) + 128);
        }
    }
    frame.unmap();
    return frame;
}

void X11ScreenRecorder::fail(const QString &message)
{
    if (m_done) {
        return;
    }
    m_done = true;
    m_error = message;
    m_timer.stop();
    if (m_recorder && m_recorder->recorderState() != QMediaRecorder::StoppedState) {
        m_recorder->stop();
    }
    sonicWarning() << "recording failed:" << message;
    Q_EMIT failed(message);
}

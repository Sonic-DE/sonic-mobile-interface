// SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

// Records the X11 screen until SIGTERM or SIGINT, then finishes the file and exits.
// Prints the file's path on the first line of standard output once recording runs.
// Exit codes: 0 saved, 1 failed, 2 no encoder available.

#include "x11screenrecorder.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSocketNotifier>

#include <KFileUtils>

#include <Sonic/Logging/Logging>

#include <xcb/randr.h>

#include <csignal>
#include <cstdio>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>

SONIC_LOGGING_COMPONENT("org.sonicde.mobile.screenrecorder")

using namespace Qt::StringLiterals;

namespace {
int s_signalFd[2] = {-1, -1};

void onSignal(int)
{
    const char byte = 1;
    [[maybe_unused]] const auto written = ::write(s_signalFd[0], &byte, 1);
}

// The area of the RandR output called name, or the whole root window when it isn't found.
QRect screenArea(const QString &name)
{
    xcb_connection_t *connection = xcb_connect(nullptr, nullptr);
    if (xcb_connection_has_error(connection)) {
        xcb_disconnect(connection);
        return {};
    }
    const xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    QRect area(0, 0, screen->width_in_pixels, screen->height_in_pixels);
    xcb_randr_get_screen_resources_current_reply_t *resources = name.isEmpty()
        ? nullptr
        : xcb_randr_get_screen_resources_current_reply(connection, xcb_randr_get_screen_resources_current(connection, screen->root), nullptr);
    if (resources) {
        const xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_current_outputs(resources);
        const int count = xcb_randr_get_screen_resources_current_outputs_length(resources);
        for (int i = 0; i < count; ++i) {
            xcb_randr_get_output_info_reply_t *output =
                xcb_randr_get_output_info_reply(connection, xcb_randr_get_output_info(connection, outputs[i], resources->config_timestamp), nullptr);
            if (!output) {
                continue;
            }
            const QString outputName =
                QString::fromUtf8(reinterpret_cast<const char *>(xcb_randr_get_output_info_name(output)), xcb_randr_get_output_info_name_length(output));
            if (outputName == name && output->crtc != XCB_NONE) {
                xcb_randr_get_crtc_info_reply_t *crtc =
                    xcb_randr_get_crtc_info_reply(connection, xcb_randr_get_crtc_info(connection, output->crtc, resources->config_timestamp), nullptr);
                if (crtc) {
                    area = QRect(crtc->x, crtc->y, crtc->width, crtc->height);
                    std::free(crtc);
                }
            }
            std::free(output);
        }
        std::free(resources);
    }
    xcb_disconnect(connection);
    return area;
}
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Sonic::Logging::install();
    app.setApplicationName(u"sonic-mobile-screenrecorder"_s);

    QCommandLineParser parser;
    parser.addHelpOption();
    const QCommandLineOption outputOption(u"output"_s, u"Where to save the recording, without a file name suffix."_s, u"path"_s);
    const QCommandLineOption screenOption(u"screen"_s, u"The output to record; the whole screen when missing."_s, u"name"_s);
    parser.addOptions({outputOption, screenOption});
    parser.process(app);
    if (!parser.isSet(outputOption)) {
        parser.showHelp(1);
    }

    const std::optional<X11ScreenRecorder::Format> format = X11ScreenRecorder::availableFormat();
    if (!format) {
        sonicWarning() << "no video encoder available";
        return 2;
    }

    QString file = parser.value(outputOption) + u'.' + format->suffix;
    const QFileInfo info(file);
    if (!QDir().mkpath(info.absolutePath())) {
        sonicWarning() << "cannot create" << info.absolutePath();
        return 1;
    }
    if (info.exists()) {
        file = info.absolutePath() + u'/' + KFileUtils::suggestName(QUrl::fromLocalFile(info.absolutePath()), info.fileName());
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, s_signalFd) != 0) {
        sonicWarning() << "cannot create the signal socket";
        return 1;
    }
    QSocketNotifier notifier(s_signalFd[1], QSocketNotifier::Read);
    std::signal(SIGTERM, onSignal);
    std::signal(SIGINT, onSignal);

    X11ScreenRecorder recorder;
    int exitCode = 0;
    QObject::connect(&notifier, &QSocketNotifier::activated, &recorder, [&] {
        char byte;
        [[maybe_unused]] const auto bytes = ::read(s_signalFd[1], &byte, 1);
        sonicInfo() << "stopping";
        recorder.finish();
    });
    QObject::connect(&recorder, &X11ScreenRecorder::finished, &app, [&] {
        sonicInfo() << "saved" << file;
        app.exit(0);
    });
    QObject::connect(&recorder, &X11ScreenRecorder::failed, &app, [&] {
        exitCode = 1;
        app.exit(1);
    });

    if (!recorder.start(file, screenArea(parser.value(screenOption)), *format)) {
        sonicWarning() << "cannot start recording:" << recorder.error();
        return 1;
    }
    std::fputs(qPrintable(file + u'\n'), stdout);
    std::fflush(stdout);

    const int result = app.exec();
    return exitCode ? exitCode : result;
}

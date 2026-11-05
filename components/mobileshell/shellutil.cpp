/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2018 Bhushan Shah <bshah@kde.org>
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "shellutil.h"

#include <KConfigGroup>
#include <KFileUtils>
#include <KLocalizedString>
#include <KNotification>
#include <KNotificationJobUiDelegate>

#include <QQuickWindow>

#include <QDBusPendingReply>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QProcess>

#include <QTextDocumentFragment>

#include <xcb/shape.h>
#include <xcb/xcb.h>

#define FORMAT24H "HH:mm:ss"

namespace {
constexpr auto inputRegionProperty = "_sonicmobile_inputRegion";

void applyInputRegion(QWindow *window)
{
    const auto x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    // An input transparent window has an empty input shape, which the region must not replace.
    if (!x11App || (window->flags() & Qt::WindowTransparentForInput)) {
        return;
    }

    xcb_connection_t *connection = x11App->connection();
    const QRect region = window->property(inputRegionProperty).toRect();
    if (region.isEmpty()) {
        xcb_shape_mask(connection, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, window->winId(), 0, 0, XCB_PIXMAP_NONE);
    } else {
        const qreal scale = window->devicePixelRatio();
        const xcb_rectangle_t rectangle{static_cast<int16_t>(qRound(region.x() * scale)),
                                        static_cast<int16_t>(qRound(region.y() * scale)),
                                        static_cast<uint16_t>(qRound(region.width() * scale)),
                                        static_cast<uint16_t>(qRound(region.height() * scale))};
        xcb_shape_rectangles(connection, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT, XCB_CLIP_ORDERING_UNSORTED, window->winId(), 0, 0, 1, &rectangle);
    }
    xcb_flush(connection);
}
}

ShellUtil::ShellUtil(QObject *parent)
    : QObject{parent}
    , m_localeConfig{KSharedConfig::openConfig(QStringLiteral("kdeglobals"))}
{
}

void ShellUtil::stackItemBefore(QQuickItem *item1, QQuickItem *item2)
{
    if (!item1 || !item2 || item1 == item2 || item1->parentItem() != item2->parentItem()) {
        return;
    }

    item1->stackBefore(item2);
}

void ShellUtil::stackItemAfter(QQuickItem *item1, QQuickItem *item2)
{
    if (!item1 || !item2 || item1 == item2 || item1->parentItem() != item2->parentItem()) {
        return;
    }

    item1->stackAfter(item2);
}

void ShellUtil::executeCommand(const QString &command)
{
    qWarning() << "Executing" << command;
    const QStringList commandAndArguments = QProcess::splitCommand(command);
    QProcess::startDetached(commandAndArguments.front(), commandAndArguments.mid(1));
}

bool ShellUtil::isSystem24HourFormat()
{
    // only load the config watcher if this function is actually used once
    if (!m_localeConfigWatcher) {
        m_localeConfigWatcher = KConfigWatcher::create(m_localeConfig);

        // watch for changes to locale config, to update 12/24 hour time
        connect(m_localeConfigWatcher.data(), &KConfigWatcher::configChanged, this, [this](const KConfigGroup &group) -> void {
            if (group.name() == "Locale") {
                // we have to reparse for new changes (from system settings)
                m_localeConfig->reparseConfiguration();
                Q_EMIT isSystem24HourFormatChanged();
            }
        });
    }

    KConfigGroup localeSettings = KConfigGroup(m_localeConfig, "Locale");

    QString timeFormat = localeSettings.readEntry("TimeFormat", QStringLiteral(FORMAT24H));
    return timeFormat == QStringLiteral(FORMAT24H);
}

void ShellUtil::launchApp(const QString &storageId)
{
    KService::Ptr service = KService::serviceByStorageId(storageId);
    if (!service) {
        qWarning() << "Could not find" << storageId;
        return;
    }

    auto job = new KIO::ApplicationLauncherJob(service, this);
    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
    job->start();
}

void ShellUtil::setInputTransparent(QQuickWindow *window, bool transparent)
{
    if (!window) {
        return;
    }

    Qt::WindowFlags flags = window->flags();
    if (transparent) {
        flags |= Qt::WindowTransparentForInput;
    } else {
        flags &= ~Qt::WindowTransparentForInput;
    }
    window->setFlags(flags);

    // Qt resets the input shape when it clears the transparency.
    if (!transparent) {
        applyInputRegion(window);
    }
}

void ShellUtil::setInputRegion(QWindow *window, const QRect &region)
{
    if (!window) {
        return;
    }

    window->setProperty(inputRegionProperty, region);
    applyInputRegion(window);
}

QString ShellUtil::toPlainText(QString htmlString)
{
    return QTextDocumentFragment::fromHtml(htmlString).toPlainText();
}

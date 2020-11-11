/***************************************************************************
 *   Copyright (C) 2015 Marco Martin <mart@kde.org>                        *
 *   Copyright (C) 2018 Bhushan Shah <bshah@kde.org>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "phonepanel.h"

#include <fcntl.h>
#include <unistd.h>

#include <QDateTime>
#include <QDBusPendingReply>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>
#include <QScreen>

constexpr int SCREENSHOT_DELAY = 200;
const char[] FLASH_SYSFS_PATH = "/sys/devices/platform/led-controller/leds/white:flash/brightness";
PhonePanel::PhonePanel(QObject *parent, const QVariantList &args)
    : Plasma::Containment(parent, args)
{
    //setHasConfigurationInterface(true);
    m_kscreenInterface = new org::kde::KScreen(QStringLiteral("org.kde.kded5"), QStringLiteral("/modules/kscreen"), QDBusConnection::sessionBus(), this);
    m_screenshotInterface = new org::kde::kwin::Screenshot(QStringLiteral("org.kde.KWin"), QStringLiteral("/Screenshot"), QDBusConnection::sessionBus(), this);
}

PhonePanel::~PhonePanel() = default;

void PhonePanel::executeCommand(const QString &command)
{
    qWarning() << "Executing" << command;
    QProcess::startDetached(command);
}

void PhonePanel::toggleTorch()
{
  int fd = open(FLASH_SYSFS_PATH, O_WRONLY);

  if (fd < 0) {
    qWarning() << "Unable to open file" << FLASH_SYSFS_PATH;
    return;
  }

  write(fd, m_running ? "0" : "1", 1);
  close(fd);
}

bool PhonePanel::autoRotate()
{
    QDBusPendingReply<bool> reply = m_kscreenInterface->getAutoRotate();
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Getting auto rotate failed:" << reply.error().name() << reply.error().message();
        return false;
    } else {
        return reply.value();
    }
}

void PhonePanel::setAutoRotate(bool value)
{
    QDBusPendingReply<> reply = m_kscreenInterface->setAutoRotate(value);
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Setting auto rotate failed:" << reply.error().name() << reply.error().message();
    } else {
        emit autoRotateChanged(value);
    }
}

void PhonePanel::takeScreenshot()
{
    // wait ~200 ms to wait for rest of animations
    QTimer::singleShot(SCREENSHOT_DELAY, [=]() {
        // screenshot fullscreen currently doesn't work on all devices -> we need to use screenshot area
        // this won't work with multiple screens
        QSize screenSize = QGuiApplication::primaryScreen()->size();
        QDBusPendingReply<QString> reply = m_screenshotInterface->screenshotArea(0, 0, screenSize.width(), screenSize.height());
        auto *watcher = new QDBusPendingCallWatcher(reply, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QString> reply = *watcher;

            if (reply.isError()) {
                qWarning() << "Creating the screenshot failed:" << reply.error().name() << reply.error().message();
            } else {
                QString filePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
                if (filePath.isEmpty()) {
                    qWarning() << "Couldn't find a writable location for the screenshot! The screenshot is in /tmp.";
                    return;
                }

                QDir picturesDir(filePath);
                if (!picturesDir.mkpath(QStringLiteral("Screenshots"))) {
                    qWarning() << "Couldn't create folder at"
                            << picturesDir.path() + QStringLiteral("/Screenshots")
                            << "to take screenshot.";
                    return;
                }

                filePath += QStringLiteral("/Screenshots/Screenshot_%1.png")
                                .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_hhmmss")));

                const QString currentPath = reply.argumentAt<0>();
                QtConcurrent::run(QThreadPool::globalInstance(), [=]() {
                    QFile screenshotFile(currentPath);
                    if (!screenshotFile.rename(filePath)) {
                        qWarning() << "Couldn't move screenshot into Pictures folder:"
                                << screenshotFile.errorString();
                    }

                    qDebug() << "Successfully saved screenshot at" << filePath;
                });
            }

            watcher->deleteLater();
        });
    });
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(quicksettings, PhonePanel, "metadata.json")

#include "phonepanel.moc"

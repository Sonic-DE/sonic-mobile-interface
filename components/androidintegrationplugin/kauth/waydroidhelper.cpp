/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KAuth/ActionReply>
#include <KAuth/HelperSupport>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <qloggingcategory.h>
#include <qprocess.h>

#define WAYDROID_COMMAND "waydroid"

using namespace Qt::StringLiterals;

class WaydroidHelper : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    KAuth::ActionReply initialize(const QVariantMap &args);
};

KAuth::ActionReply WaydroidHelper::initialize(const QVariantMap &args)
{
    QString systemType = args.value(u"systemType"_s).toString();
    QString romType = args.value(u"romType"_s).toString();
    bool forced = args.value(u"forced"_s, false).toBool();

    QStringList arguments;
    arguments << "init";
    arguments << "-s" << systemType;
    arguments << "-r" << romType;
    if (forced) {
        arguments << "-f";
    }

    QProcess *process = new QProcess(this);
    process->start(WAYDROID_COMMAND, arguments);
    process->waitForFinished();

    if (process->exitCode() == 0) {
        return KAuth::ActionReply::SuccessReply();
    } else {
        qWarning() << "Failed to initialize Waydroid: " << process->readAllStandardError();
        return KAuth::ActionReply::HelperErrorReply();
    }
}

KAUTH_HELPER_MAIN("org.kde.plasma.mobileshell.waydroidhelper", WaydroidHelper)

#include "waydroidhelper.moc"
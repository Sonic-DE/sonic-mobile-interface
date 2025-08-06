/*
 *   SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QDir>
#include <QObject>
#include <qtmetamacros.h>

class WaydroidApplication : public QObject, public std::enable_shared_from_this<WaydroidApplication>
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString packageName READ packageName CONSTANT)

public:
    typedef std::shared_ptr<WaydroidApplication> Ptr;

    explicit WaydroidApplication(QObject *parent = nullptr);

    /**
     * @brief Read one application from "waydroid app list" command output log through QTextStream.
     * The QTextStream cursor must be set to the first line of the application.
     * The first line begin with "Name:".
     *
     * @param inFile The QTextStream used to read line by line the Waydroid logs.
     * @return One parsed application
     */
    static WaydroidApplication::Ptr fromWaydroidLog(QTextStream &inFile);

    /**
     * @brief Read all applications from "waydroid app list" command output log through QTextStream.
     *
     * @param inFile The QTextStream used to read line by line the Waydroid logs.
     * @return All parsed applications
     */
    static QList<WaydroidApplication::Ptr> parseApplicationsFromWaydroidLog(QTextStream &inFile);

    QString name() const;
    QString packageName() const;

private:
    QString m_name;
    QString m_packageName;
};

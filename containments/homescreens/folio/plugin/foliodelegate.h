// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>

#include "folioapplication.h"
#include "folioapplicationfolder.h"

class FolioDelegate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FolioDelegate::Type type READ type CONSTANT)
    Q_PROPERTY(FolioApplication *application READ application CONSTANT)
    Q_PROPERTY(FolioApplicationFolder *folder READ folder CONSTANT)

public:
    enum Type {
        None,
        Application,
        Folder,
    };
    Q_ENUM(Type)

    FolioDelegate(FolioApplication *application = nullptr, FolioApplicationFolder *folder = nullptr, QObject *parent = nullptr);

    FolioDelegate::Type type();
    FolioApplication *application();
    FolioApplicationFolder *folder();

private:
    FolioDelegate::Type m_type;
    FolioApplication *m_application;
    FolioApplicationFolder *m_folder;
};

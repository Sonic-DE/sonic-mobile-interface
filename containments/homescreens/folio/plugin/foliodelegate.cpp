// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "foliodelegate.h"

FolioDelegate::FolioDelegate(FolioApplication *application, FolioApplicationFolder *folder, QObject *parent)
    : QObject{parent}
    , m_type{FolioDelegate::None}
    , m_application{application}
    , m_folder{folder}
{
    if (application) {
        m_type = FolioDelegate::Application;
    }
    if (folder) {
        m_type = FolioDelegate::Folder;
    }
}

FolioDelegate::Type FolioDelegate::type()
{
    return m_type;
}

FolioApplication *FolioDelegate::application()
{
    return m_application;
}

FolioApplicationFolder *FolioDelegate::folder()
{
    return m_folder;
}

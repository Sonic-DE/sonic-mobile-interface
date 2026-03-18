
// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "folioapplication.h"
#include "windowlistener.h"

#include <QQuickWindow>

#include <KNotificationJobUiDelegate>

FolioApplication::FolioApplication(KService::Ptr service, QObject *parent)
    : QObject{parent}
    , m_running{false}
    , m_name{service->name()}
    , m_icon{service->icon()}
    , m_storageId{service->storageId()}
{
    if (service->property<bool>(QStringLiteral("X-KDE-PlasmaMobile-UseGenericName"))) {
        m_name = service->genericName();
    }
}

FolioApplication::Ptr FolioApplication::fromJson(QJsonObject &obj)
{
    QString storageId = obj[QStringLiteral("storageId")].toString();
    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        return std::make_shared<FolioApplication>(service);
    }
    return nullptr;
}

QJsonObject FolioApplication::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("type")] = "application";
    obj[QStringLiteral("storageId")] = m_storageId;
    return obj;
}

bool FolioApplication::running() const
{
    return false;
}

QString FolioApplication::name() const
{
    return m_name;
}

QString FolioApplication::icon() const
{
    return m_icon;
}

QString FolioApplication::storageId() const
{
    return m_storageId;
}

void FolioApplication::setName(QString &name)
{
    m_name = name;
    Q_EMIT nameChanged();
}

void FolioApplication::setIcon(QString &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged();
}

void FolioApplication::setStorageId(QString &storageId)
{
    m_storageId = storageId;
    Q_EMIT storageIdChanged();
}

void FolioApplication::setMinimizedDelegate(QQuickItem *delegate)
{
    (void)delegate;
}

void FolioApplication::unsetMinimizedDelegate(QQuickItem *delegate)
{
    (void)delegate;
}

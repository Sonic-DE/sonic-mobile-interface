// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "folioapplication.h"
#include "windowlistener.h"

#include <QFileInfo>
#include <QIcon>
#include <QQuickWindow>
#include <QUrl>

static bool isIconValid(const QString &iconNameOrPath)
{
    if (iconNameOrPath.isEmpty()) {
        return false;
    }

    if (iconNameOrPath.startsWith(QStringLiteral("file://"))) {
        return QFileInfo::exists(QUrl(iconNameOrPath).toLocalFile());
    } else if (iconNameOrPath.startsWith(QLatin1Char('/'))) {
        return QFileInfo::exists(iconNameOrPath);
    }

    return QIcon::hasThemeIcon(iconNameOrPath);
}

FolioApplication::FolioApplication(KService::Ptr service, const QStringList &categories, QObject *parent)
    : QObject{parent}
    , m_running{false}
    , m_name{service ? service->name() : QString()}
    , m_icon{service && isIconValid(service->icon()) ? service->icon() : QStringLiteral("unknown")}
    , m_storageId{service ? service->storageId() : QString()}
    , m_categories{categories}
    , m_service{service}
{
    if (service && service->property<bool>(QStringLiteral("X-KDE-PlasmaMobile-UseGenericName"))) {
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

QStringList FolioApplication::categories() const
{
    return m_categories;
}

QString FolioApplication::storageId() const
{
    return m_storageId;
}

KService::Ptr FolioApplication::service() const
{
    return m_service;
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

void FolioApplication::setCategories(const QStringList &categories)
{
    if (m_categories != categories) {
        m_categories = categories;
        Q_EMIT categoriesChanged();
    }
}

void FolioApplication::setMinimizedDelegate(QQuickItem *delegate)
{
    Q_UNUSED(delegate);
}

void FolioApplication::unsetMinimizedDelegate(QQuickItem *delegate)
{
    Q_UNUSED(delegate);
}

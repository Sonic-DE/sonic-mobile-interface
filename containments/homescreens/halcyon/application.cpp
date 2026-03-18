// SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "application.h"
#include "windowlistener.h"

#include <QQuickWindow>

#include <KNotificationJobUiDelegate>

Application::Application(QObject *parent, KService::Ptr service)
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

Application *Application::fromJson(QJsonObject &obj, QObject *parent)
{
    QString storageId = obj[QStringLiteral("storageId")].toString();
    if (KService::Ptr service = KService::serviceByStorageId(storageId)) {
        return new Application(parent, service);
    }
    return nullptr;
}

QJsonObject Application::toJson()
{
    QJsonObject obj;
    obj[QStringLiteral("type")] = "application";
    obj[QStringLiteral("storageId")] = m_storageId;
    return obj;
}

bool Application::running() const
{
    return false;
}

QString Application::name() const
{
    return m_name;
}

QString Application::icon() const
{
    return m_icon;
}

QString Application::storageId() const
{
    return m_storageId;
}

void Application::setName(QString &name)
{
    m_name = name;
    Q_EMIT nameChanged();
}

void Application::setIcon(QString &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged();
}

void Application::setStorageId(QString &storageId)
{
    m_storageId = storageId;
    Q_EMIT storageIdChanged();
}

void Application::setMinimizedDelegate(QQuickItem *delegate)
{
    (void)delegate;
}

void Application::unsetMinimizedDelegate(QQuickItem *delegate)
{
    (void)delegate;
}

/**
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <KPluginFactory>

#include <KConfigGroup>
#include <KQuickManagedConfigModule>
#include <KSharedConfig>

#include "mobileshellsettings.h"

class KCMMobileShell : public KQuickManagedConfigModule
{
    Q_OBJECT

    Q_PROPERTY(MobileShellSettings *Settings READ config CONSTANT)

public:
    KCMMobileShell(QObject *parent, const KPluginMetaData &data)
        : KQuickManagedConfigModule(parent, data),
          m_config(new MobileShellSettings(this))
    {
        setButtons({});
        qmlRegisterAnonymousType<MobileShellSettings>("Settings", 1);
    }

    MobileShellSettings *config() const {
        return m_config;
    }

Q_SIGNALS:
    void navigationPanelEnabledChanged();

private:
    MobileShellSettings *m_config;
};

K_PLUGIN_CLASS_WITH_JSON(KCMMobileShell, "kcm_mobileshell.json")

#include "kcm.moc"

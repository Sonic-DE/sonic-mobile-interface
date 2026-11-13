// SPDX-FileCopyrightText: 2023 by Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "prepareutil.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <kscreen/configmonitor.h>
#include <kscreen/getconfigoperation.h>
#include <kscreen/output.h>
#include <kscreen/setconfigoperation.h>

#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>
#include <QProcess>

PrepareUtil::PrepareUtil(QObject *parent)
    : QObject{parent}
{
    initKScreen([]() { });

    // set property initially
    m_usingDarkTheme = KConfigGroup(KSharedConfig::openConfig(QStringLiteral("plasmamobilerc")), QStringLiteral("General")).readEntry("darkMode", false);
}

void PrepareUtil::initKScreen(std::function<void()> callback)
{
    connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished, this, [this, callback](auto *op) {
        m_config = qobject_cast<KScreen::GetConfigOperation *>(op)->config();
        if (!m_config) {
            qDebug() << "PrepareUtil: Failed to get kscreen config, attempting again";
            initKScreen(callback);
            return;
        }

        KScreen::ConfigMonitor::instance()->addConfig(m_config);

        int scaling = 100;

        // To determine the scaling value:
        // Try to take the primary display's scaling, otherwise use the scaling of any of the displays
        int lowestPriority = std::numeric_limits<unsigned int>::max();
        Q_UNUSED(lowestPriority);
        for (KScreen::OutputPtr output : m_config->outputs()) {
            if (!output) {
                continue;
            }
            scaling = output->scale() * 100;
            m_output = output->id();
            if (output->priority() == 1) {
                break;
            }
        }

        m_scaling = scaling;
        Q_EMIT scalingChanged();

        callback();
    });
}

int PrepareUtil::scaling() const
{
    return m_scaling;
}

void PrepareUtil::setScaling(int scaling)
{
    if (!m_config) {
        initKScreen([this, scaling]() {
            setScalingInternal(scaling);
        });
        return;
    }

    setScalingInternal(scaling);
}

void PrepareUtil::setScalingInternal(int scaling)
{
    const auto outputs = m_config->outputs();
    qreal scalingNum = ((double)scaling) / 100;

    for (KScreen::OutputPtr output : outputs) {
        if (!output) {
            continue;
        }
        if (output->id() == m_output) {
            output->setScale(scalingNum);
        }
    }

    auto setop = new KScreen::SetConfigOperation(m_config, this);
    setop->exec();

    m_scaling = scaling;
    Q_EMIT scalingChanged();
}

QStringList PrepareUtil::scalingOptions()
{
    return {"50%", "75%", "100%", "125%", "150%", "175%", "200%", "225%", "250%", "275%", "300%"};
}

bool PrepareUtil::usingDarkTheme() const
{
    return m_usingDarkTheme;
}

void PrepareUtil::setUsingDarkTheme(bool usingDarkTheme)
{
    // The session's own theme, as the Dark Mode quick setting sets it; the desktop keeps its own.
    auto config = KSharedConfig::openConfig(QStringLiteral("plasmamobilerc"));
    KConfigGroup(config, QStringLiteral("General")).writeEntry("darkMode", usingDarkTheme, KConfig::Notify);
    config->sync();
    const int result = QProcess::execute(QStringLiteral("plasma-mobile-envmanager"), {QStringLiteral("--apply-settings")});
    if (result != 0) {
        qWarning() << "Failed to apply the session theme, exit status" << result;
        return;
    }

    m_usingDarkTheme = usingDarkTheme;
    Q_EMIT usingDarkThemeChanged();
}

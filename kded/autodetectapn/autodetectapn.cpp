// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDE-Accepted-GPL

#include <KIO/CommandLauncherJob>
#include <KNotificationJobUiDelegate>
#include <KPluginFactory>
#include <KUser>

#include <QDBusReply>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QStandardPaths>

#include <NetworkManagerQt/CdmaSetting>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

#include <ModemManagerQt/Manager>
#include <ModemManagerQt/Modem3Gpp>

#include "autodetectapn.h"

K_PLUGIN_FACTORY_WITH_JSON(StartFactory, "kded_plasma_mobile_autodetectapn.json", registerPlugin<AutoDetectAPN>();)

AutoDetectAPN::AutoDetectAPN(QObject *parent, const QList<QVariant> &)
    : KDEDModule{parent}
{
    checkAndAddAutodetectedAPN();
}

QCoro::Task<void> AutoDetectAPN::checkAndAddAutodetectedAPN()
{
    qDebug() << QStringLiteral("Running APN autodetection...");

    for (ModemManager::ModemDevice::Ptr mmDevice : ModemManager::modemDevices()) {
        ModemManager::Modem::Ptr mmModem = mmDevice->modemInterface();

        if (!mmModem) {
            continue;
        }

        ModemManager::Modem3gpp::Ptr mm3gppDevice = mmDevice->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
        NetworkManager::ModemDevice::Ptr nmModem = findNMModem(mmModem);
        ModemManager::Sim::Ptr mmSim = mmDevice->sim();

        if (!mm3gppDevice || !nmModem || !mmSim) {
            continue;
        }

        // Detect whether the modem already has an APN
        // TODO: currently just check if there are any NM connections, this doesn't work if the user swapped out their SIM.
        //       we need something that detects when this occurs
        if (!nmModem->availableConnections().empty()) {
            qDebug() << QStringLiteral("Modem") << nmModem->uni() << QStringLiteral("already has a connection configured");
            continue;
        }

        // MCCMNC value
        QString operatorCode = mmSim->operatorIdentifier();
        QString gid1 = mmSim->gid1(); // for carriers using MVNO, which could cause duplicate MCCMNC values
        QString spn = mmSim->operatorName();
        QString imsi = mmSim->imsi();

        // Autodetect an APN
        APNEntry detectedAPN = findAPN(operatorCode, gid1, spn, imsi);
        if (detectedAPN.apn.isEmpty()) {
            qDebug() << QStringLiteral("Could not find an APN for the SIM with code") << operatorCode;
            continue;
        }

        // Create connection
        NetworkManager::ConnectionSettings::Ptr settings{new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Gsm)};
        settings->setId(detectedAPN.carrier);
        settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        settings->setAutoconnect(true);
        settings->addToPermissions(KUser().loginName(), QString());

        NetworkManager::GsmSetting::Ptr gsmSetting = settings->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
        gsmSetting->setApn(detectedAPN.apn);
        gsmSetting->setPasswordFlags(NetworkManager::Setting::NotRequired);
        gsmSetting->setNetworkType(NetworkManager::GsmSetting::NetworkType::Prefer4GLte);
        gsmSetting->setHomeOnly(false); // TODO respect modem roaming settings?
        gsmSetting->setInitialized(true);

        QDBusReply<QDBusObjectPath> reply = co_await NetworkManager::addAndActivateConnection(settings->toMap(), nmModem->uni(), "");
        if (!reply.isValid()) {
            qWarning() << QStringLiteral("Error adding autodetected connection:") << reply.error().message();
        } else {
            qDebug() << QStringLiteral("Successfully autodetected") << detectedAPN.carrier << QStringLiteral("with APN") << detectedAPN.apn << ".";
        }
    }
}

NetworkManager::ModemDevice::Ptr AutoDetectAPN::findNMModem(ModemManager::Modem::Ptr mmModem)
{
    for (NetworkManager::Device::Ptr nmDevice : NetworkManager::networkInterfaces()) {
        if (nmDevice->udi() == mmModem->uni()) {
            return nmDevice.objectCast<NetworkManager::ModemDevice>();
        }
    }
    return nullptr;
}

AutoDetectAPN::APNEntry AutoDetectAPN::findAPN(const QString &operatorCode, const QString &gid1, const QString &spn, const QString &imsi) const
{
    const QString providersFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("apns-full-conf.xml"));
    QFile file{providersFile};

    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QDomDocument document;
    document.setContent(&file);

    QDomElement root = document.documentElement();
    if (root.isNull()) {
        return {};
    }

    QDomNode apns = root.firstChild(); // <apns ...
    if (!apns.isNull()) {
        return {};
    }

    QList<APNEntry> candidates;

    QDomNode node = apns.firstChild(); // <apn ...
    while (!node.isNull()) {
        QDomElement element = node.toElement();

        // only entries for internet
        if (!element.attribute("type").contains("default")) {
            continue;
        }

        QString mccmnc = element.attribute("mcc") + element.attribute("mnc");

        if (mccmnc == operatorCode) {
            APNEntry entry{element.attribute("apn"), element.attribute("carrier")};
            candidates.push_back(entry);

            // check if we have an MVNO match and prioritize that
            if ((!gid1.isEmpty() && element.attribute("mvno_type") == "gid" && element.attribute("mvno_match_data") == gid1)
                || (!spn.isEmpty() && element.attribute("mvno_type") == "spn" && element.attribute("mvno_match_data") == spn)
                || (!imsi.isEmpty() && element.attribute("mvno_type") == "imsi" && imsi.startsWith(element.attribute("mvno_match_data")))) {
                return entry;
            }
        }

        node = node.nextSibling();
    }

    if (candidates.size() > 0) {
        return candidates[0];
    } else {
        return {};
    }
}

#include "autodetectapn.moc"


/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 * SparkFun Data Logger - setup methods
 *
 */
#include "flxApplication.h"

#include <Flux/flxDevSerial.h>

//---------------------------------------------------------------------
// Setup the IOT clients
bool flxApplication::setupIoTClients()
{
#if defined(CONFIG_IOT_MQTT) || defined(CONFIG_IOT_ARDUINO) || defined(CONFIG_IOT_AWS) ||                              \
    defined(CONFIG_IOT_THINGSPEAK) || defined(CONFIG_IOT_AZURE) || defined(CONFIG_IOT_HTTP) ||                         \
    defined(CONFIG_IOT_MACHINECHAT)

    _iotEndpoints.setTitle("Services");
    _iotEndpoints.setName("IoT Services", "IoT Service Connection Drivers");

#if defined(CONFIG_IOT_MQTT)
    // Add title for this section
    _mqttClient.setTitle("IoT Services");
    // setup the network connection for the mqtt
    _mqttClient.setNetwork(&_wifiConnection);
    // add mqtt to JSON
    _fmtJSON.add(_mqttClient);
    _iotEndpoints.push_back(_mqttClient);

    // setup the network connection for the mqtt
    _mqttSecureClient.setNetwork(&_wifiConnection);
    // add mqtt to JSON
    _fmtJSON.add(_mqttSecureClient);

    _iotEndpoints.push_back(_mqttSecureClient);
#endif

#if defined(CONFIG_IOT_AWS)
    // AWS
    _iotAWS.setName("AWS IoT", "Connect to an AWS Iot Thing");
    _iotAWS.setNetwork(&_wifiConnection);

#if defined(CONFIG_FLUX_SDMMCARD)
    // Add the filesystem to load certs/keys from the SD card
    _iotAWS.setFileSystem(&_theSDCard);
#endif
    _fmtJSON.add(_iotAWS);

    _iotEndpoints.push_back(_iotAWS);
#endif

#if defined(CONFIG_IOT_THINGSPEAK)
    // Thingspeak driver
    _iotThingSpeak.setNetwork(&_wifiConnection);

#if defined(CONFIG_FLUX_SDMMCARD)
    // Add the filesystem to load certs/keys from the SD card
    _iotThingSpeak.setFileSystem(&_theSDCard);
#endif
    _fmtJSON.add(_iotThingSpeak);

    // Add the ThingSpeak driver to the flux system
    _iotEndpoints.push_back(_iotThingSpeak);
#endif

#if defined(CONFIG_IOT_AZURE)
    // Azure IoT
    _iotAzure.setNetwork(&_wifiConnection);

    // Add the filesystem to load certs/keys from the SD card
#if defined(CONFIG_FLUX_SDMMCARD)
    _iotAzure.setFileSystem(&_theSDCard);
#endif
    _fmtJSON.add(_iotAzure);

    // Add the Azure IoT driver to the flux system
    _iotEndpoints.push_back(_iotAzure);
#endif

#if defined(CONFIG_IOT_HTTP)
    // general HTTP / URL logger
    _iotHTTP.setNetwork(&_wifiConnection);

    // TODO _ find a better method for the filesystem identifier
#if defined(CONFIG_FLUX_SDMMCARD)
    _iotHTTP.setFileSystem(&_theSDCard);
#endif
    _fmtJSON.add(_iotHTTP);

    // Add the HTTP driver to the flux system
    _iotEndpoints.push_back(_iotHTTP);
#endif
#endif

    return true;
}

//---------------------------------------------------------------------------
// setupTime()
//
// Setup any time sources/sinks. Called after devices are loaded

bool flxApplication::setupTime()
{

#if defined(CONFIG_FLUX_CLOCK)
    // what is our clock - as setup from init/prefs
    std::string refClock = flxClock.referenceClock();

#if defined(CONFIG_FLUX_NTP)
    flxClock.addReferenceClock(&_ntpClient, _ntpClient.name());

#endif
#endif
#if defined(CONFIG_DEVICE_GNSS)
    // Any GNSS devices attached?
    auto allGNSS = flux.get<flxDevGNSS>();
    for (auto gnss : *allGNSS)
        flxClock.addReferenceClock(gnss, gnss->name());
#endif

#if defined(CONFIG_DEVICE_RV8003)
    // RTC clock?
    auto allRTC8803 = flux.get<flxDevRV8803>();
    for (auto rtc8803 : *allRTC8803)
    {
        flxClock.addReferenceClock(rtc8803, rtc8803->name());
        flxClock.addConnectedClock(rtc8803);
    }

#endif
    // Now that clocks are loaded, set the ref clock to what was started with.
    flxClock.referenceClock = refClock;

    // update the system clock to the reference clock
    flxClock.updateClock();

    return true;
}

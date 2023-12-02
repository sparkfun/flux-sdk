/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#ifdef ESP32

#include "flxWiFiESP32.h"
#include "flxSerial.h"

// ESP32 library...
#include <WiFiMulti.h>

#include <esp_log.h>
#include <esp_wifi.h>
// WiFi client for ESP32 boards

// Define a connection iteration value - exceed this, skip the connection

#define kMaxConnectionTries 10

//----------------------------------------------------------------
// Enabled Property setter/getters
void flxWiFiESP32::set_isEnabled(bool bEnabled)
{
    // Any changes?
    if (_isEnabled == bEnabled)
        return;

    // changing state ...
    // TODO - Should this control connection state?

    _isEnabled = bEnabled;

    if (_isEnabled)
        (void)connect();
    else
        disconnect();
}

//----------------------------------------------------------------
bool flxWiFiESP32::get_isEnabled(void)
{
    return _isEnabled;
}

//----------------------------------------------------------------
// Connection Management

bool flxWiFiESP32::connect(void)
{

    if (!_isEnabled)
        return false;

    // If we are already connected, return
    if (WiFi.isConnected())
        return true;

    WiFi.mode(WIFI_STA);

    // Make sure we have the correct protocols set for WiFi (modes/ wifi types)
    uint8_t protocols = 0;
    esp_err_t response = esp_wifi_get_protocol(WIFI_IF_STA, &protocols);
    if (response != ESP_OK)
        flxLog_W(F("%s: Failed to get wifi protocols: %s\r\n"), name(), esp_err_to_name(response));

    else if (protocols != (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N))
    {
        response = esp_wifi_set_protocol(WIFI_IF_STA,
                                         WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N); // Enable WiFi.
        if (response != ESP_OK)
            flxLog_W(F("%s: Error setting WiFi protocols: %s\r\n"), esp_err_to_name(response));
    }

    WiFiMulti wifiMulti;

    // make sure

    int nNet = 0;

    //  Do we have credentials?

    // Add the access points the user has provided
    if (SSID().length() != 0)
    {
        nNet++;
        wifiMulti.addAP(SSID().c_str(), password().c_str());
    }
    if (alt1_SSID().length() != 0)
    {
        nNet++;
        wifiMulti.addAP(alt1_SSID().c_str(), alt1_password().c_str());
    }

    if (alt2_SSID().length() != 0)
    {
        nNet++;
        wifiMulti.addAP(alt2_SSID().c_str(), alt2_password().c_str());
    }

    if (alt3_SSID().length() != 0)
    {
        nNet++;
        wifiMulti.addAP(alt3_SSID().c_str(), alt3_password().c_str());
    }
    if (nNet == 0)
    {
        flxLog_E(F("WiFi: No credentials provided. Unable to connect"));
        WiFi.mode(WIFI_OFF);
        return false;
    }

    flxLog_I_(F("Connecting to WiFi..."));

    // May 2023
    // the ESP32 WiFi system throws out errors with some APs - all harmless, confusing
    // So, disable output messages from the ESP32 error system when we connect
    //
    // This is fixed in the IDF https://github.com/espressif/esp-idf/issues/9283
    //
    // TODO - revisit this
    esp_log_level_t level = esp_log_level_get("wifi");
    esp_log_level_set("wifi", ESP_LOG_NONE);

    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        flxLog_N_(F("."));
        i++;
        if (i > kMaxConnectionTries)
        {
            flxLog_E_(F("Unable to connect to a provided WiFi access point. Verify AP availability and credentials."));

            // TODO: Revisit
            // back to normal esp logging
            esp_log_level_set("wifi", level);

            WiFi.disconnect(true);
            return false;
        }
    }
    // TODO: Revisit
    // back to normal esp logging
    esp_log_level_set("wifi", level);

    flxSerial.textToWhite();
    flxLog_N(F("Connected to %s"), WiFi.SSID().c_str());
    flxSerial.textToNormal();

    // okay, we're connected.
    _wasConnected = true;
    on_connectionChange.emit(true);

    return true;
}

//----------------------------------------------------------------
void flxWiFiESP32::disconnect(void)
{

    if (WiFi.isConnected())
    {
        if (!WiFi.disconnect(true))
        {
            flxLog_E(F("WiFi disconnect() - error disconnecting"));
        }
    }
    if (_wasConnected)
        on_connectionChange.emit(false);

    _wasConnected = false;
}

//----------------------------------------------------------------
bool flxWiFiESP32::isConnected()
{
    return (_isEnabled && WiFi.isConnected());
}

//----------------------------------------------------------------
bool flxWiFiESP32::initialize(void)
{
    // is it desired to delay the startup/connect call?
    if (_delayedStartup)
    {
        _delayedStartup = false;
        return false;
    }
    return connect();
}

String flxWiFiESP32::connectedSSID(void)
{
    return WiFi.SSID();
}

//----------------------------------------------------------------
bool flxWiFiESP32::loop(void)
{
    // Connection change???
    if (_isEnabled)
    {
        bool wifiConn = WiFi.isConnected();
        if (wifiConn != _wasConnected)
        {
            _wasConnected = wifiConn;
            on_connectionChange.emit(_wasConnected);
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------
// return an abstract rating of the Wifi
uint flxWiFiESP32::rating(void)
{
    if (!_isEnabled || !WiFi.isConnected())
        return kWiFiLevelPoor;

    int8_t rssi = WiFi.RSSI();

    return rssi > -40   ? kWiFiLevelExcellent
           : rssi > -60 ? kWiFiLevelGood
           : rssi > -80 ? kWiFiLevelFair
                        : kWiFiLevelPoor;
}

#endif
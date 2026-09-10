/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
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
        flxLogM_E(kMsgErrValueNotProvided, name(), "Connection Credentials");
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
    // esp_log_level_t level = esp_log_level_get("wifi");
    // esp_log_level_set("wifi", ESP_LOG_NONE);

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
            // esp_log_level_set("wifi", level);

            WiFi.disconnect(true);

            // The initial connect failed - but that must not be terminal. A
            // remote device commonly powers up before its access point does.
            // Queue the update job anyway so the reconnect logic in
            // jobHandlerCB() keeps trying. (addJob() is idempotent.)
            flxAddJobToQueue(_theJob);
            return false;
        }
    }
    // TODO: Revisit
    // back to normal esp logging
    // esp_log_level_set("wifi", level);

    flxSerial.textToWhite();
    flxLog_N(F("Connected to %s"), WiFi.SSID().c_str());
    flxSerial.textToNormal();

    // okay, we're connected.
    _wasConnected = true;

    // Healthy connection - start the reconnect backoff over.
    _lastReconnectMS = 0;
    _reconnectDelayMS = kWiFiReconnectDelayMinMS;
    _reconnectIndex = 0;

    flxSendEvent(flxEvent::kOnConnectionChange, true);

    flxAddJobToQueue(_theJob);
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
        flxSendEvent(flxEvent::kOnConnectionChange, false);

    _wasConnected = false;

    _lastReconnectMS = 0;
    _reconnectDelayMS = kWiFiReconnectDelayMinMS;
    _reconnectIndex = 0;

    flxRemoveJobFromQueue(_theJob);
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
        return true;
    }
    return connect();
}

String flxWiFiESP32::connectedSSID(void)
{
    return WiFi.SSID();
}

//----------------------------------------------------------------
// How many networks has the user given us credentials for?
uint flxWiFiESP32::nCredentials(void)
{
    uint nCreds = 0;

    if (SSID().length() != 0)
        nCreds++;
    if (alt1_SSID().length() != 0)
        nCreds++;
    if (alt2_SSID().length() != 0)
        nCreds++;
    if (alt3_SSID().length() != 0)
        nCreds++;

    return nCreds;
}

//----------------------------------------------------------------
// Return the nth configured credential pair, skipping any empty slots.
bool flxWiFiESP32::credentialsAt(uint index, std::string &theSSID, std::string &thePassword)
{
    std::string theSSIDs[4] = {SSID(), alt1_SSID(), alt2_SSID(), alt3_SSID()};
    std::string thePasswords[4] = {password(), alt1_password(), alt2_password(), alt3_password()};

    uint nFound = 0;

    for (uint i = 0; i < 4; i++)
    {
        if (theSSIDs[i].length() == 0)
            continue;

        if (nFound == index)
        {
            theSSID = theSSIDs[i];
            thePassword = thePasswords[i];
            return true;
        }
        nFound++;
    }

    return false;
}

//----------------------------------------------------------------
// beginReconnect()
//
// Start - but do not wait on - a reconnection attempt.
//
// This deliberately does not use WiFiMulti. WiFiMulti::run() calls
// WiFi.begin(ssid, pass, channel, bssid), which sets bssid_set in the stored
// station config, pinning this device to one AP radio on one channel for the
// life of the boot. Every later auto-reconnect reuses that config, so if the
// BSSID or channel changes - AP reboot, auto-channel move, randomized BSSID -
// the core retries a target that no longer exists. Calling begin() with only
// the SSID clears that pin and lets the supplicant use what is on the air now.
//
// Returns true if an attempt was started. The result is picked up by a later
// pass of the job handler, so the main loop is never blocked here.
bool flxWiFiESP32::beginReconnect(void)
{
    uint nCreds = nCredentials();

    if (nCreds == 0)
        return false;

    std::string theSSID;
    std::string thePassword;

    // Rotate through the configured networks so an unavailable primary does
    // not lock out the alternates.
    if (!credentialsAt(_reconnectIndex % nCreds, theSSID, thePassword))
        return false;

    _reconnectIndex++;

    flxLog_I(F("%s: connection lost - reconnecting to %s"), name(), theSSID.c_str());

    // Note: WiFi.begin() returns the station's *current* status, not the outcome
    // of dispatching this attempt. After a preceding failure that can still read
    // WL_CONNECT_FAILED while the new attempt is under way perfectly happily, so
    // it cannot be used as a success signal. Having credentials and having called
    // begin() is what "started" means here - the job handler observes the real
    // result on a later pass.
    WiFi.begin(theSSID.c_str(), thePassword.c_str());

    return true;
}

//----------------------------------------------------------------
void flxWiFiESP32::jobHandlerCB(void)
{
    if (!_isEnabled)
        return;

    bool wifiConn = WiFi.isConnected();

    // Connection change???
    if (wifiConn != _wasConnected)
    {
        _wasConnected = wifiConn;
        flxSendEvent(flxEvent::kOnConnectionChange, _wasConnected);
    }

    if (wifiConn)
    {
        // Healthy - start the backoff over so the next outage retries promptly.
        _lastReconnectMS = 0;
        _reconnectDelayMS = kWiFiReconnectDelayMinMS;
        _reconnectIndex = 0;
        return;
    }

    // Not connected. The ESP32 core may have abandoned auto-reconnect for good
    // (see the note in flxWiFiESP32.h), and nothing else retries - so we do.
    uint32_t ticks = millis();

    if (_lastReconnectMS != 0 && (ticks - _lastReconnectMS) < _reconnectDelayMS)
        return;

    _lastReconnectMS = ticks;

    if (!beginReconnect())
        flxLogM_E(kMsgErrValueNotProvided, name(), "Connection Credentials");

    // Grow the backoff on every attempt - a successful connection resets it
    // above. This keeps a device that is out of range from hammering the radio
    // every few seconds indefinitely.
    _reconnectDelayMS *= 2;

    if (_reconnectDelayMS > kWiFiReconnectDelayMaxMS)
        _reconnectDelayMS = kWiFiReconnectDelayMaxMS;
}
//----------------------------------------------------------------
// return an abstract rating of the WiFi
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
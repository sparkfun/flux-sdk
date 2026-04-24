/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */
#pragma once

#include <ArduinoJson.h>
#include <Flux/flxApplication.h>

#include "flxPlatform.h"
#include <Flux/flxCoreLog.h>
#include <Flux/flxSerialField.h>
#include <Flux/flxUtils.h>
#include <time.h>

#include "flxAppCommands.h"

static const char *kSeparator = "---------------------------------------------------------------------";

void flxAppCommands::factoryResetDevice(void *arg)
{
    flxApplication *theApp = static_cast<flxApplication *>(arg);
    if (!theApp)
        return;

    theApp->_sysUpdate.factoryResetDevice();
}

//---------------------------------------------------------------------
void flxAppCommands::resetDevice(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);

    // Need to prompt for an a-okay ...
    Serial.printf("\n\rClear and Restart Device? [Y/n]? ");
    uint8_t selected = theApp->_serialSettings.getMenuSelectionYN();
    flxLog_N("");

    if (selected != 'y' || selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
    {
        flxLog_I(F("Aborting..."));
        return;
    }

    resetDeviceForced(theApp);
}
//---------------------------------------------------------------------
void flxAppCommands::resetDeviceForced(void *arg)
{
    if (!arg)
        return;
    flxApplication *theApp = static_cast<flxApplication *>(arg);

    theApp->_sysStorage.resetStorage();
    flxLog_I(F("Settings Cleared"));

    theApp->_sysUpdate.restartDevice();
}
//---------------------------------------------------------------------
void flxAppCommands::clearDeviceSettings(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);

    // Need to prompt for an a-okay ...
    Serial.printf("\n\rClear Device Saved Settings? [Y/n]? ");
    uint8_t selected = theApp->_serialSettings.getMenuSelectionYN();
    flxLog_N("");

    if (selected != 'y' || selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
    {
        flxLog_I(F("Aborting..."));
        return;
    }
    clearDeviceSettingsForced(theApp);
}

//---------------------------------------------------------------------
void flxAppCommands::clearDeviceSettingsForced(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);

    theApp->_sysStorage.resetStorage();
    flxLog_I(F("Settings Cleared"));
}
//---------------------------------------------------------------------
void flxAppCommands::restartDevice(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);
    theApp->_sysUpdate.restartDevicePrompt();
}
//---------------------------------------------------------------------
void flxAppCommands::restartDeviceForced(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);
    theApp->_sysUpdate.restartDevice();
}
//---------------------------------------------------------------------
void flxAppCommands::aboutDevice(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);
    theApp->sysAbout();
}
//---------------------------------------------------------------------
///
/// @brief Reads JSON from the serial console - uses as input into the settings system
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
// void flxAppCommands::loadJSONSettings(void *arg)
// {
//     if (!theApp)
//         return ;

//     // Create a JSON prefs serial object and read in the settings
//     flxStorageJSONPrefSerial prefsSerial(flxSettings.fallbackBuffer() > 0 ? flxSettings.fallbackBuffer() : 2000);

//     // restore the settings from serial
//     bool status = flxSettings.restoreObjectFromStorage(&flux, &prefsSerial);
//     if (!status)
//         return ;

//     flxLog_I_(F("Settings restored from serial..."));

//     // now save the new settings in primary storage
//     status = flxSettings.save(&flux, true);
//     if (status)
//         flxLog_N(F("saved locally"));

//
// }
//---------------------------------------------------------------------
///
/// @brief Saves the current system to preferences/Settings
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::saveSettings(void *arg)
{

    // Just call save
    bool status = flxSettings.save(&flux);
    if (status)
        flxLog_I(F("Saving System Settings."));
    else
        flxLog_E(F("Error saving settings"));
}
//---------------------------------------------------------------------
///
/// @brief Dumps out the current heap size/stats
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::heapStatus(void *arg)
{
    uint32_t totalHeap = flxPlatform::heap_size();
    uint32_t freeHeap = flxPlatform::heap_free();

    // just dump out the current heap
    flxLog_I(F("System Heap - Total: %dB Free: %dB (%.1f%%)"), totalHeap, freeHeap,
             (float)freeHeap / (float)totalHeap * 100.);
}
//---------------------------------------------------------------------
///
/// @brief Enables verbose log level output
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::logLevelVerbose(void *arg)
{
    flxLog.setLogLevel(flxLogVerbose);
    flxLog_V(F("Output level set to Verbose"));
}
//---------------------------------------------------------------------
///
/// @brief Toggle verbose output
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::toggleVerboseOutput(void *arg)
{

    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);
    theApp->set_verbose(!theApp->get_verbose());
    flxLog_I("Verbose Output %s", theApp->get_verbose() ? "Enabled" : "Disabled");
}
//---------------------------------------------------------------------
///
/// @brief Dumps out the current logging rate metric
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::logRateStats(void *arg)
{
    if (!arg)
        return;
    flxApplication *theApp = static_cast<flxApplication *>(arg);

    // Run rate metric
    flxLog_N_(F("Logging Rate - Set Interval: %u (ms)  Measured: "), theApp->_timer.interval());
    if (!theApp->_logger.enabledLogRate())
        flxLog_N("%s", "<disabled>");
    else
        flxLog_N("%.2f (ms)", theApp->_logger.getLogRate());
}

//---------------------------------------------------------------------
///
/// @brief Toggles the state of current logging rate metric
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::logRateToggle(void *arg)
{
    if (!arg)
        return;
    flxApplication *theApp = static_cast<flxApplication *>(arg);
    theApp->_logger.logRateMetric = !theApp->_logger.logRateMetric();
    // Run rate metric
    flxLog_N(F("Logging Rate Metric %s"), theApp->_logger.enabledLogRate() ? "Enabled" : "Disabled");
}
//---------------------------------------------------------------------
///
/// @brief Dumps out the current wifi stats
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::wifiStats(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);

    if (theApp->_wifiConnection.enabled() && theApp->_wifiConnection.isConnected())
    {
        IPAddress addr = theApp->_wifiConnection.localIP();
        uint rating = theApp->_wifiConnection.rating();
        const char *szRSSI = rating == kWiFiLevelExcellent ? "Excellent"
                             : rating == kWiFiLevelGood    ? "Good"
                             : rating == kWiFiLevelFair    ? "Fair"
                                                           : "Weak";
        flxLog_I(F("WiFi - Connected  SSID: %s  IP Address: %d.%d.%d.%d  Signal: %s"),
                 theApp->_wifiConnection.connectedSSID().c_str(), addr[0], addr[1], addr[2], addr[3], szRSSI);
    }
    else
        flxLog_I(F("WiFi - Not Connected/Enabled"));
}
//---------------------------------------------------------------------
///
/// @brief Dumps out the current sd card stats
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
// void flxAppCommands::sdCardStats(void *arg)
// {
//     if (!theApp)
//         return false;

//     if (theApp->_theSDCard.enabled())
//     {

//         char szSize[32];
//         char szCap[32];
//         char szAvail[32];

//         flx_utils::formatByteString(theApp->_theSDCard.size(), 2, szSize, sizeof(szSize));
//         flx_utils::formatByteString(theApp->_theSDCard.total(), 2, szCap, sizeof(szCap));
//         flx_utils::formatByteString(theApp->_theSDCard.total() - theApp->_theSDCard.used(), 2, szAvail,
//                                     sizeof(szAvail));

//         flxLog_I(F("SD Card - Type: %s Size: %s Capacity: %s Free: %s (%.1f%%)"), theApp->_theSDCard.type(),
//         szSize,
//                  szCap, szAvail, 100. - (theApp->_theSDCard.used() / (float)theApp->_theSDCard.total() * 100.));
//     }
//     else
//         flxLog_I(F("SD card not available"));

//     return true;
// }

//---------------------------------------------------------------------
///
/// @brief Lists loaded devices
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::listLoadedDevices(void *arg)
{

    // connected devices...
    flxDeviceContainer myDevices = flux.connectedDevices();
    flxLog_I(F("Connected Devices [%d]:"), myDevices.size());

    // Loop over the device list - note that it is iterable.
    for (auto device : myDevices)
    {
        flxLog_N_(F("    %-20s  - %s  {"), device->name(), device->description());
        if (device->getKind() == flxDeviceKindI2C)
            flxLog_N("%s x%x}", "qwiic", device->address());
        else
            flxLog_N("%s p%u}", "SPI", device->address());
    }
}
//---------------------------------------------------------------------
///
/// @brief outputs current time
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::outputSystemTime(void *arg)
{

    char szBuffer[64];
    memset(szBuffer, '\0', sizeof(szBuffer));
    time_t t_now;
    time(&t_now);

    flx_utils::timestampISO8601(t_now, szBuffer, sizeof(szBuffer), true);

    flxLog_I("%s", szBuffer);
    ;
}

//---------------------------------------------------------------------
///
/// @brief outputs uptime
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::outputUpTime(void *arg)
{

    // uptime
    uint32_t days, hours, minutes, secs, mills;

    flx_utils::uptime(days, hours, minutes, secs, mills);

    flxLog_I("Uptime: %u days, %02u:%02u:%02u.%u", days, hours, minutes, secs, mills);
}
//---------------------------------------------------------------------
///
/// @brief log an observation now!
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::logObservationNow(void *arg)
{

    flxSendEvent(flxEvent::kOnLogObservationWithSource, "CLI");
}

//---------------------------------------------------------------------
///
/// @brief output the firmware version
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::printVersion(void *arg)
{

    char szBuffer[128];
    flux.versionString(szBuffer, sizeof(szBuffer), true);

    flxLog_I("%s   %s", flux.name(), flux.description());
    flxLog_I("Version: %s\n\r", szBuffer);
}
//---------------------------------------------------------------------
///
/// @brief output the device ID
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::printDeviceID(void *arg)
{

    flxLog_I("Device ID: %s", flux.deviceId());
}
//---------------------------------------------------------------------
///
/// @brief output the build date of the firmware
///
/// @param theApp Pointer to the DataLogger App
/// @retval bool indicates success (true) or failure (!true)
///
void flxAppCommands::printBuildDate(void *arg)
{
    if (!arg)
        return;

    flxApplication *theApp = static_cast<flxApplication *>(arg);
    flxLog_I("Build Date: %s", theApp->getBuildDate());
}

void flxAppCommands::_printTimeStamp(void)
{
    char szBuffer[64] = {'\0'};

    time_t t_now;
    time(&t_now);
    flx_utils::timestampISO8601(t_now, szBuffer, sizeof(szBuffer), false);
    flxLog_N_("%s", szBuffer);
}
void flxAppCommands::printTimeStamp(void *arg)
{
    _printTimeStamp();
    flxLog_N("");

    char szBuffer[64] = {'\0'};
}
void flxAppCommands::printTimeStampBreak(void *arg)
{
    _printTimeStamp();
    printBreakLine(arg);
}
void flxAppCommands::printBreakLine(void *arg)
{
    flxLog_N(kSeparator);
}

void flxAppCommands::clearConsole(void *arg)
{
    flxLog_N_("\033[2J\033[H"); // ANSI escape codes to clear the console
}

bool flxAppCommands::processCommand(void *arg)
{
    // The data editor we're using - serial field
    flxSerialField theDataEditor;
    std::string sBuffer;
    bool status = theDataEditor.editFieldString(sBuffer);

    flxLog_N(""); // need to output a CR

    if (!status)
        return false;

    // cleanup string
    sBuffer = flx_utils::strtrim(sBuffer);

    // is this for help?
    if (sBuffer == "help")
    {
        listAllCommands();
        return true;
    }
    // Find our command
    status = hasCommand(sBuffer);
    if (status)
        status = callCommand(sBuffer, arg);
    else
        flxLog_N(F("Unknown Command: `%s`"), sBuffer.c_str());

    return status;
}

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

// Create a command to function call registry class. Basically a method registry
// class.

using flxCmdRegFn = std::function<void(void *)>;
using flxCmdRegMap = std::map<std::string, flxCmdRegFn>;

class flxCmdRegistry
{
  public:
    // --- Registration --------------------------------------------------------

    // lambda, std::function, or instance method wrapper
    void registerCommand(const std::string &id, flxCmdRegFn fn)
    {
        if (_commandMap.count(id))
            flxLog_W(F("Overwriting command: %s"), id.c_str());
        _commandMap[id] = std::move(fn);
    }

    // static method or free C function
    void registerCommand(const std::string &id, void (*ptr)(void *))
    {
        if (_commandMap.count(id))
            flxLog_W(F("Overwriting command: %s"), id.c_str());
        _commandMap[id] = ptr;
    }

    // --- Removal -------------------------------------------------------------

    void unregisterCommand(const std::string &id)
    {
        auto it = _commandMap.find(id);
        if (it != _commandMap.end())
            _commandMap.erase(it);
    }

    // --- Invocation ----------------------------------------------------------
    bool callCommand(const std::string &id, void *args)
    {

        auto it = _commandMap.find(id);
        bool status = (it == _commandMap.end());
        if (status)
            flxLog_E("Unknown command: !%s", id);
        else
            it->second(args);

        return status;
    }

    // --- Introspection -------------------------------------------------------

    bool hasCommand(const std::string &id) const
    {
        return _commandMap.count(id) > 0;
    }

    size_t size() const
    {
        return _commandMap.size();
    }

    void listAllCommands() const
    {
        for (auto it : _commandMap)
            flxLog_N(F("   !%s"), it.first.c_str());
    }

  private:
    flxCmdRegMap _commandMap;
};

class flxAppCommands : public flxCmdRegistry
{

    //---------------------------------------------------------------------
    // Command Callbacks
    //---------------------------------------------------------------------
    static void factoryResetDevice(void *arg);

    static void resetDevice(void *arg);

    static void resetDeviceForced(void *arg);
    static void clearDeviceSettings(void *arg);

    static void clearDeviceSettingsForced(void *arg);

    static void restartDevice(void *arg);

    static void restartDeviceForced(void *arg);

    static void aboutDevice(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief Reads JSON from the serial console - uses as input into the settings system
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    // static bool loadJSONSettings(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief Saves the current system to preferences/Settings
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void saveSettings(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Dumps out the current heap size/stats
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void heapStatus(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Enables verbose log level output
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void logLevelVerbose(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Toggle verbose output
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void toggleVerboseOutput(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Dumps out the current logging rate metric
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void logRateStats(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief Toggles the state of current logging rate metric
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void logRateToggle(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Dumps out the current wifi stats
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void wifiStats(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief Dumps out the current sd card stats
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    // static void sdCardStats(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief Lists loaded devices
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void listLoadedDevices(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief outputs current time
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void outputSystemTime(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief outputs uptime
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void outputUpTime(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief log an observation now!
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void logObservationNow(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief output the firmware version
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void printVersion(void *arg);

    //---------------------------------------------------------------------
    ///
    /// @brief output the device ID
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void printDeviceID(void *arg);
    //---------------------------------------------------------------------
    ///
    /// @brief output the build date of the firmware
    ///
    /// @param theApp Pointer to the DataLogger App
    /// @retval bool indicates success (true) or failure (!true)
    ///
    static void printBuildDate(void *arg);

    static void _printTimeStamp(void);
    static void printTimeStamp(void *arg);
    static void printTimeStampBreak(void *arg);

    static void printBreakLine(void *arg);

    static void clearConsole(void *arg);

  public:
    flxAppCommands()
    {
        registerCommand("factory-reset", &flxAppCommands::factoryResetDevice);
        registerCommand("reset-device", &flxAppCommands::resetDevice);
        registerCommand("reset-device-forced", &flxAppCommands::resetDeviceForced);
        registerCommand("clear-settings", &flxAppCommands::clearDeviceSettings);
        registerCommand("clear-settings-forced", &flxAppCommands::clearDeviceSettingsForced);
        registerCommand("restart", &flxAppCommands::restartDevice);
        registerCommand("restart-forced", &flxAppCommands::restartDeviceForced);
        // registerCommand("json-settings", &flxAppCommands::loadJSONSettings);
        registerCommand("log-rate", &flxAppCommands::logRateStats);
        registerCommand("log-rate-toggle", &flxAppCommands::logRateToggle);
        registerCommand("log-now", &flxAppCommands::logObservationNow);
        registerCommand("l", &flxAppCommands::logObservationNow);
        registerCommand("wifi", &flxAppCommands::wifiStats);
        // registerCommand("sdcard", &flxAppCommands::sdCardStats);
        registerCommand("devices", &flxAppCommands::listLoadedDevices);
        registerCommand("save-settings", &flxAppCommands::saveSettings);
        registerCommand("heap", &flxAppCommands::heapStatus);
        registerCommand("h", &flxAppCommands::heapStatus);
        registerCommand("verbose", &flxAppCommands::toggleVerboseOutput);
        registerCommand("systime", &flxAppCommands::outputSystemTime);
        registerCommand("uptime", &flxAppCommands::outputUpTime);
        registerCommand("device-id", &flxAppCommands::printDeviceID);
        registerCommand("version", &flxAppCommands::printVersion);
        registerCommand("build-date", &flxAppCommands::printBuildDate);
        registerCommand("time", &flxAppCommands::printTimeStamp);
        registerCommand("t", &flxAppCommands::printTimeStamp);
        registerCommand("break", &flxAppCommands::printBreakLine);
        registerCommand("b", &flxAppCommands::printBreakLine);
        registerCommand("timebreak", &flxAppCommands::printTimeStampBreak);
        registerCommand("tb", &flxAppCommands::printTimeStampBreak);
        registerCommand("clear-console", &flxAppCommands::clearConsole);
        registerCommand("c", &flxAppCommands::clearConsole);

        registerCommand("about", &flxAppCommands::aboutDevice);
    }
    bool processCommand(void *arg);
};

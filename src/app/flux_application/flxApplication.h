/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 * SparkFun Foundation App - logger
 *
 */
#pragma once

// Core Items
#include <Flux.h>
#include <Flux/flxCoreJobs.h>
#include <Flux/flxCoreLog.h>
#include <Flux/flxSerial.h>
#include <Flux/flxTimer.h>

// Logging Module
#if defined(CONFIG_FLUX_LOGGING)
#include <Flux/flxFmtCSV.h>
#include <Flux/flxFmtJSON.h>
#include <Flux/flxLogger.h>
#endif

// Prefs Module
#if defined(CONFIG_FLUX_PREFS)
#include <Flux/flxPreferences.h>
#include <Flux/flxSettings.h>
#endif

// Serial Settings editor module
#if defined(CONFIG_FLUX_PREFS_SERIAL)
#include <Flux/flxSettingsSerial.h>
#endif

// JSON backed prefs module
#if defined(CONFIG_FLUX_PREFS_JSON)
#include <Flux/flxStorageJSONPref.h>
#endif

// NTP Module?
#if defined(CONFIG_FLUX_NTP)
// TODO - work on platform abstraction
#ifdef ESP32
#include <Flux/flxNTPESP32.h>
#endif
#endif

#if defined(CONFIG_FLUX_WIFI)
// TODO - work on platform abstraction
#ifdef ESP32
#include <Flux/flxWiFiESP32.h>
#endif
#endif

// IoT Client Includes
#if defined(CONFIG_IOT_AWS)
#include <Flux/flxIoTAWS.h>
#endif

#if defined(CONFIG_IOT_ARDUINO)
#include <Flux/flxIoTArduino.h>
#endif

#if defined(CONFIG_IOT_AZURE)
#include <Flux/flxIoTAzure.h>
#endif

#if defined(CONFIG_IOT_HTTP)
#include <Flux/flxIoTHTTP.h>
#endif

#if defined(CONFIG_IOT_MACHINECHAT)
#include <Flux/flxIoTMachineChat.h>
#endif

#if defined(CONFIG_IOT_THINGSPEAK)
#include <Flux/flxIoTThingSpeak.h>
#endif

#if defined(CONFIG_IOT_MQTT)
// TODO - work in platform abstraction
#ifdef ESP32
#include <Flux/flxMQTTESP32.h>
#endif
#endif

// External Serial Device connection and use
#if defined(CONFIG_OPT_EXTSERIAL)
#include <Flux/flxOptExtSerial.h>
#endif

// Interrupt event to drive logging
#if defined(CONFIG_OPT_EXTINTERRUPT)
#include <Flux/flxOptInterruptEvent.h>
#endif

// System Firmware update/reset
#if defined(CONFIG_FLUX_FIRMWARE)
#include <Flux/flxSysFirmware.h>
#endif

// LED?
#if defined(CONFIG_FLUX_APP_LED)
#include <Flux/flxAppLED.h>
#endif
#if defined(CONFIG_FLUX_APP_LEDRGB)
#include <Flux/flxAppLEDRGB.h>
#endif

// the board definitions
#include <flux_board_config.h>

#if defined(CONFIG_FLUX_APP_BUTTON)
#include <Flux/flxAppButton.h>
#endif

#include <utility>

//------------------------------------------
// Default log interval in milli secs
const uint16_t kDefaultLogInterval = 15000;

// Buffer size of our JSON document output
const uint16_t kAppJSONDocSize = 1600;

// NTP Startup delay in secs
const uint16_t kAppNTPStartupDelaySecs = 5;

// Default Sleep Periods
const uint16_t kSystemSleepSleepSec = 60;
const uint16_t kSystemSleepWakeSec = 120;

// What is the out of the box baud rate ..
const uint32_t kDefaultTerminalBaudRate = 115200;

// General startup delay (secs) for the startup menu
const uint32_t kStartupMenuDefaultDelaySecs = 2;

// Operation mode flags
#define kFlxApplicationOpNone (0)
#define kFlxApplicationOpEditing (1 << 0)
#define kFlxApplicationOpStartup (1 << 1)
#define kFlxApplicationOpPendingRestart (1 << 2)

// startup things
#define kFlxApplicationOpStartNoAutoload (1 << 3)
#define kFlxApplicationOpStartListDevices (1 << 4)
#define kFlxApplicationOpStartNoSettings (1 << 5)
#define kFlxApplicationOpStartNoWiFi (1 << 6)
#define kAppOpStartVerboseOutput (1 << 7)

#define kFlxApplicationOpStartAllFlags                                                                                 \
    (kFlxApplicationOpStartNoAutoload | kFlxApplicationOpStartListDevices | kFlxApplicationOpStartNoSettings |         \
     kFlxApplicationOpStartNoWiFi | kAppOpStartVerboseOutput)

#define inOpMode(__mode__) ((_opFlags & __mode__) == __mode__)
#define setOpMode(__mode__) _opFlags |= __mode__
#define clearOpMode(__mode__) _opFlags &= ~__mode__
//
// forward declare of our system info class
class flxAppSystemInfo;
// flxApplication
//-------------------------------------------------------------------------
// Define our application class the config based application
//-------------------------------------------------------------------------

class flxApplication : public flxApplicationBase
{
  private:
    //---------------------------------------------------------------------
    // Setup the IOT clients
    bool setupIoTClients(void);

    //---------------------------------------------------------------------
    // Setup time ...
    bool setupTime(void);

  public:
    //---------------------------------------------------------------------------
    // Constructor
    //

    flxApplication();

    //---------------------------------------------------------------------------
    // onSetup()
    //
    // Called by the system before devices are loaded, and system initialized
    bool sysSetup() final;

    //---------------------------------------------------------------------------
    // onDeviceLoad()
    //
    // Called by the system, right after device auto-load, but before system restore
    // Allows the app to load other devices.
    void sysDeviceLoad(void) final;

    void sysRestore(void) final;

    void resetDevice(void);

  private:
#if defined(CONFIG_FLUX_APP_COMMANDS)
    friend class flxAppCommands;
#endif
    friend class flxAppSystemInfo;

    //------------------------------------------
    // For controlling the log output types

    static constexpr uint8_t kAppLogTypeNone = 0x0;
    static constexpr uint8_t kAppLogTypeCSV = 0x1;
    static constexpr uint8_t kAppLogTypeJSON = 0x2;

    static constexpr char *kLogFormatNames[] = {"Disabled", "CSV Format", "JSON Format"};

    // Startup output modes
    static constexpr uint8_t kAppStartupMsgNormal = 0x0;
    static constexpr uint8_t kAppStartupMsgCompact = 0x1;
    static constexpr uint8_t kAppStartupMsgNone = 0x2;

    //---------------------------------------------------------------------------
    uint8_t get_logTypeSer(void);

    //---------------------------------------------------------------------------
    void set_logTypeSer(uint8_t logType);

    void about_app_status(void)
    {
        displayAppAbout();
    }
    uint8_t _logTypeSer;

    // For the terminal baud rate setting

    uint32_t _terminalBaudRate;

    uint32_t get_termBaudRate(void);
    void set_termBaudRate(uint32_t rate);

#if defined(CONFIG_FLUX_APP_LED)
    bool get_ledEnabled(void);
    void set_ledEnabled(bool);
#endif

    bool get_sleepEnabled(void);
    void set_sleepEnabled(bool);

    uint32_t get_sleepWakePeriod(void);
    void set_sleepWakePeriod(uint32_t);

    uint32_t get_jsonBufferSize(void);
    void set_jsonBufferSize(uint32_t);

    bool get_verbose_dev_name(void);
    void set_verbose_dev_name(bool);

    std::string get_local_name(void);
    void set_local_name(std::string name);

    bool get_verbose(void);
    void set_verbose(bool enable);

    // color text
    bool get_color_text(void);
    void set_color_text(bool);

    // for enabling system info in the log stream

    bool get_logsysinfo(void);
    void set_logsysinfo(bool);

    // support for onInit
    void onInitStartupCommands(uint);

  public:
    //---------------------------------------------------------------------------

    // sysInit()
    //
    // Called before anything is started
    void sysInit() final;

    // sysStart()
    //
    // Called after the system is loaded, restored and initialized
    bool sysStart() final;

    bool loop();

    // Define our log type properties

    flxPropertyRWUInt8<flxApplication, &flxApplication::get_logTypeSer, &flxApplication::set_logTypeSer> serialLogType =
        {kAppLogTypeCSV,
         {{kLogFormatNames[kAppLogTypeNone], kAppLogTypeNone},
          {kLogFormatNames[kAppLogTypeCSV], kAppLogTypeCSV},
          {kLogFormatNames[kAppLogTypeJSON], kAppLogTypeJSON}}};

    // JSON output buffer size
    flxPropertyRWUInt32<flxApplication, &flxApplication::get_jsonBufferSize, &flxApplication::set_jsonBufferSize>
        jsonBufferSize = {100, 5000};

    // System sleep properties
    flxPropertyUInt32<flxApplication> sleepInterval = {5, 86400};
    flxPropertyRWUInt32<flxApplication, &flxApplication::get_sleepWakePeriod, &flxApplication::set_sleepWakePeriod>
        wakeInterval = {60, 86400};
    flxPropertyRWBool<flxApplication, &flxApplication::get_sleepEnabled, &flxApplication::set_sleepEnabled>
        sleepEnabled;

#if defined(CONFIG_FLUX_APP_LED)
    // Display LED Enabled?
    flxPropertyRWBool<flxApplication, &flxApplication::get_ledEnabled, &flxApplication::set_ledEnabled> ledEnabled;

#if defined(CONFIG_FLUX_APP_LEDRGB)
    flxAppLEDRGB theLED;
#else
    flxAppLED theLED;
#endif
#endif

    // Serial Baud rate setting
    flxPropertyRWUInt32<flxApplication, &flxApplication::get_termBaudRate, &flxApplication::set_termBaudRate>
        serialBaudRate = {1200, 500000};

    // Verbose Device Names
    flxPropertyRWBool<flxApplication, &flxApplication::get_verbose_dev_name, &flxApplication::set_verbose_dev_name>
        verboseDevNames;

    flxParameterInVoid<flxApplication, &flxApplication::about_app_status> aboutApplication;

    // board user set name
    flxPropertyRWString<flxApplication, &flxApplication::get_local_name, &flxApplication::set_local_name>
        localBoardName;

    // Color Text Output
    flxPropertyRWBool<flxApplication, &flxApplication::get_color_text, &flxApplication::set_color_text>
        colorTextOutput = {true};

    // startup delay setting
    flxPropertyUInt32<flxApplication> startupDelaySecs = {0, 60};

    flxPropertyUInt8<flxApplication> startupOutputMode = {
        kAppStartupMsgNormal,
        {{"Normal", kAppStartupMsgNormal}, {"Compact", kAppStartupMsgCompact}, {"Disabled", kAppStartupMsgNone}}};

    // Verbose messages enabled?
    flxPropertyRWBool<flxApplication, &flxApplication::get_verbose, &flxApplication::set_verbose> verboseEnabled = {
        false};

    // log system info
    // Enabled/Disabled
    flxPropertyRWBool<flxApplication, &flxApplication::get_logsysinfo, &flxApplication::set_logsysinfo> logSysInfo;

    void onSystemActivity(void);
    void onSystemActivityLow(void);

  protected:
    void enterSleepMode(void);

    void _displayAboutObjHelper(char, const char *, bool);
    void displayAppAbout(void);
    void displayAppStatus(bool useInfo = false);

    // event things
#if defined(CONFIG_FLUX_FIRMWARE)
    void onFirmwareLoad(bool bLoading);
#endif
#if defined(CONFIG_FLUX_PREFS_SERIAL)
    void onSettingsEdit(bool bLoading);
#endif

    void onErrorMessage(uint8_t);

    void getStartupProperties(uint32_t &baudRate, uint32_t &startupDelay);

#if defined(CONFIG_FLUX_APP_BUTTON)
    // Board button callbacks
    void onButtonPressed(uint32_t);
    void onButtonReleased(uint32_t);
#endif

    // system device add/remove events -- when this happens, bookkeeping is requires
    void onDeviceAdded(uint32_t);
    void onDeviceRemoved(uint32_t);

    // helpful method to get the build time of the firmware - used in the about and status displays
    const char *getBuildDate(void);
    // Class members -- that make up the application structure

    // WiFi and NTP
#if defined(CONFIG_FLUX_WIFI)
    flxWiFiESP32 _wifiConnection;
#endif

#if defined(CONFIG_FLUX_NTP)
    flxNTPESP32 _ntpClient;
#endif

    // Create a JSON and CSV output formatters.
    // Note: setting internal buffer sizes using template to minimize alloc calls.
#if defined(CONFIG_FLUX_LOGGING)
    flxFormatJSON<kAppJSONDocSize> _fmtJSON;
    flxFormatCSV _fmtCSV;

    // Our logger
    flxLogger _logger;

#endif

    // Timer for event logging
    flxTimer _timer;

#if defined(CONFIG_FLUX_SDMMCARD)
    // SD Card Filesystem object
    flxFSSDMMCard _theSDCard;
#endif

#if defined(CONFIG_FLUX_PREFS)
    // settings things
    flxPreferences _sysStorage;
#endif

#if defined(CONFIG_FLUX_PREFS_SERIAL)
    flxSettingsSerial _serialSettings;
#endif

#if defined(CONFIG_FLUX_PREFS_JSON)
    flxStorageJSONPrefFile _jsonStorage;
#endif

#if defined(CONFIG_OPT_EXTSERIAL)
    // the external serial connection manager
    flxOptExtSerial _extSerial;
#endif

#if defined(CONFIG_OPT_EXTINTERRUPT)
    // interrupt event to drive logging
    flxOptInterruptEvent _extIntrEvent;
#endif

#if defined(CONFIG_IOT_MQTT) || defined(CONFIG_IOT_ARDUINO) || defined(CONFIG_IOT_AWS) ||                              \
    defined(CONFIG_IOT_THINGSPEAK) || defined(CONFIG_IOT_AZURE) || defined(CONFIG_IOT_HTTP) ||                         \
    defined(CONFIG_IOT_MACHINECHAT)
    // Container for IoT endpoint drivers
    flxActionContainer _iotEndpoints;
#endif
    // IoT endpoints

#if defined(CONFIG_IOT_MQTT)
    // An generic MQTT client
    flxMQTTESP32 _mqttClient;

    // secure mqtt
    flxMQTTESP32Secure _mqttSecureClient;
#endif

    // AWS
#if defined(CONFIG_IOT_AWS)
    flxIoTAWS _iotAWS;
#endif

#if defined(CONFIG_IOT_THINGSPEAK)
    // Thingspeak
    flxIoTThingSpeak _iotThingSpeak;
#endif

#if defined(CONFIG_IOT_AZURE)
    // azure
    flxIoTAzure _iotAzure;
#endif

#if defined(CONFIG_IOT_HTTP)
    // HTTP/URL Post
    flxIoTHTTP _iotHTTP;
#endif

    // KDB Testing
    // Web Server
    // sfeDLWebServer _iotWebServer;

#if defined(CONFIG_FLUX_FIRMWARE)
    // Our firmware Update/Reset system
    flxSysFirmware _sysUpdate;
#endif

#if defined(CONFIG_FLUX_APP_BUTTON)
    // for our button events of the board
    flxAppButton _boardButton;
#endif
  private:
    // For the sleep timer
    unsigned long _startTime = 0;

    uint32_t _modeFlags;
    uint32_t _opFlags;

    // sleep things - is enabled storage, sleep event
    bool _bSleepEnabled;
    flxJob _sleepJob;

    // log sys info

    bool _bLogSysInfo;
    flxAppSystemInfo *_pSystemInfo;
};

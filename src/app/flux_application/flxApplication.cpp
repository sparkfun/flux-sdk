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
 * Flux App Foundation - logging
 *
 */

#include "flxApplication.h"
#include "flxAppSystemInfo.h"

#if defined(CONFIG_FLUX_APP_COMMANDS)
#include "flxAppCommands.h"
#endif

#include "esp_sleep.h"

#if defined(CONFIG_FLUX_CLOCK)
// for our time setup
#include <Flux/flxClock.h>
#endif

#include <Flux/flxUtils.h>

#include <time.h>

RTC_DATA_ATTR int boot_count = 0;

// Some version checks - if these are not set ...
#ifndef FLUX_APP_VERSION_MAJOR
#define FLUX_APP_VERSION_MAJOR 0
#endif

#ifndef FLUX_APP_VERSION_MINOR
#define FLUX_APP_VERSION_MINOR 0
#endif

#ifndef FLUX_APP_VERSION_POINT
#define FLUX_APP_VERSION_POINT 0
#endif

#ifndef FLUX_APP_VERSION_DESCRIPTOR
#define FLUX_APP_VERSION_DESCRIPTOR "Version 0.0.0"
#endif

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0
#endif

#ifndef FLUX_APP_CLASS_NAME_ID
#define FLUX_APP_CLASS_NAME_ID "NIL-APP-CLASS"
#endif
// For finding the firmware files on SD card
#define kDataLoggerFirmwareFilePrefix "SparkFun_DataLoggerIoT_"

// Application keys - used to encrypt runtime secrets for the app.
//
// NOTE: Gen a base 64 key  % openssl rand -base64 32
//       Convert into ASCII ints in python %    data = [ord(c) for c in ss]
//       Jam into the below array

// If a key array is passed in via a #define, use that, otherwise use a default, dev key
#ifdef DATALOGGER_IOT_APP_KEY
static const uint8_t _app_jump[] = DATALOGGER_IOT_APP_KEY;
#else
static const uint8_t _app_jump[] = {104, 72, 67, 51,  74,  67,  108, 99, 104, 112, 77,  100, 55,  106, 56,
                                    78,  68, 69, 108, 98,  118, 51,  65, 90,  48,  51,  82,  111, 120, 56,
                                    52,  49, 70, 76,  103, 77,  84,  49, 85,  99,  117, 66,  111, 61};
#endif

// The datalogger firmware OTA manifest  URL
// Testing repo location
// #define kDataLoggerOTAManifestURL
// "https://raw.githubusercontent.com/gigapod/ota-demo-exp/main/manifiest/sfe-dl-manifest.json"

// Final/Deploy repo
#define kDataLoggerOTAManifestURL                                                                                      \
    "https://raw.githubusercontent.com/sparkfun/SparkFun_DataLogger/main/firmware/manifest/sfe-dl-manifest.json"

// Button event increment
#define kButtonPressedIncrement 5

// Startup/Timeout for serial connection to init...
#define kSerialStartupDelayMS 5000

constexpr char *flxApplication::kLogFormatNames[];

// delay used in loop during startup
const uint32_t kStartupLoopDelayMS = 70;

//---------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------
//

flxApplication::flxApplication()
    : _logTypeSer{kAppLogTypeNone}, _timer{kDefaultLogInterval}, _modeFlags{0}, _opFlags{0}, _bSleepEnabled{false},
      _bLogSysInfo{false}, _pSystemInfo{nullptr}
{

    // Add a title for this section - the application level  - of settings
    setTitle("General");

#if defined(CONFIG_FLUX_APP_LED)
    flxRegister(ledEnabled, "LED Enabled", "Enable/Disable the on-board LED activity");
#endif

    // our the menu timeout property to our props/menu system entries
    addProperty(_serialSettings.menuTimeout);
    flxRegister(colorTextOutput, "Color Output", "Use color output with the Serial console");

    // user defined board name
    flxRegister(localBoardName, "Board Name", "A specific name for this DataLogger");

    // sdCardLogType.setTitle("Output");
    // flxRegister(sdCardLogType, "SD Card Format", "Enable and set the output format");
    flxRegister(serialLogType, "Serial Console Format", "Enable and set the output format");
    flxRegister(jsonBufferSize, "JSON Buffer Size", "Output buffer size in bytes");

    // Terminal Serial Baud Rate
    flxRegister(serialBaudRate, "Terminal Baud Rate", "Update terminal baud rate. Changes take effect on restart");
    _terminalBaudRate = kDefaultTerminalBaudRate;

    // Add the format changing props to the logger - makes more sense from a UX standpoint.
    // _logger.addProperty(sdCardLogType);
    _logger.addProperty(serialLogType);

    _logger.setTitle("Logging");

    // sleep properties
    sleepEnabled.setTitle("Sleep");
    flxRegister(sleepEnabled, "Enable System Sleep", "If enabled, sleep the system ");
    flxRegister(sleepInterval, "Sleep Interval (sec)", "The interval the system will sleep for");
    flxRegister(wakeInterval, "Wake Interval (sec)", "The interval the system will operate between sleep period");

    startupOutputMode.setTitle("Advanced");
    flxRegister(startupOutputMode, "Startup Messages", "Level of message output at startup");
    flxRegister(startupDelaySecs, "Startup Delay", "Startup Menu Delay in Seconds");
    flxRegister(verboseDevNames, "Device Names", "Name always includes the device address");
    flxRegister(verboseEnabled, "Verbose Messages", "Enable verbose messages");

    // about?
    flxRegister(aboutApplication, "About...", "Details about the system");
    aboutApplication.prompt = false; // no prompt needed before execution

    // Log system info property - we host it at the app level, but only display it in the logger object
    // So *manually* register so it's not added to this objects property list - set name, desc
    logSysInfo.setName("System Info", "Log system information");
    logSysInfo(this, true); // set owner object - skip the add to prop list for this object

#if defined(CONFIG_FLUX_LOGGING)
    // add this prop to our logger
    _logger.addProperty(logSysInfo);

    // Update timer object string
    _timer.setName("Logging Timer", "Set the interval between log entries");
#endif

    // set sleep default interval && event handler method
    sleepInterval = kSystemSleepSleepSec;
    _sleepJob.setup("sleep", kSystemSleepSleepSec * 1000, this, &flxApplication::enterSleepMode, true);

    // app key
    flux.setAppToken(_app_jump, sizeof(_app_jump));

    // do not want the wifi connect() call made until after initialize
#if defined(CONFIG_FLUX_WIFI)
    _wifiConnection.setDelayedStartup(true);
#endif
}

//---------------------------------------------------------------------------
// Event callbacks
//---------------------------------------------------------------------------
// Display things during firmware loading
//---------------------------------------------------------------------------
void flxApplication::onFirmwareLoad(bool bLoading)
{
#if defined(CONFIG_FLUX_APP_LED)
    if (bLoading)
        theLED.on(flxColor::Yellow);
    else
        theLED.off();
#endif
}

//---------------------------------------------------------------------------
// Flash led on error/warnings
//---------------------------------------------------------------------------
void flxApplication::onErrorMessage(uint8_t msgType)
{
#if defined(CONFIG_FLUX_APP_LED)
    // send an LED thing
    if (msgType == (uint8_t)flxLogError)
        theLED.flash(flxColor::Red);
    else if (msgType == (uint8_t)flxLogWarning)
        theLED.flash(flxColor::Yellow);
#endif
}

#if defined(CONFIG_FLUX_PREFS_SERIAL)
//---------------------------------------------------------------------------
// Display things during settings edits
//---------------------------------------------------------------------------
void flxApplication::onSettingsEdit(bool bLoading)
{

    if (bLoading)
    {
        setOpMode(kFlxApplicationOpEditing);
#if defined(CONFIG_FLUX_APP_LED)
        theLED.on(flxColor::LightGray);
#endif
    }
    else
    {
#if defined(CONFIG_FLUX_APP_LED)
        theLED.off();
#endif

        // no longer editing
        clearOpMode(kFlxApplicationOpEditing);

        // did the editing operation set a restart flag? If so see if the user wants to restart
        // the device.
        if (inOpMode(kFlxApplicationOpPendingRestart))
        {
            flxLog_N("\n\rSome changes required a device restart to take effect...");
            _sysUpdate.restartDevice();

            // this shouldn't return unless user aborted
            clearOpMode(kFlxApplicationOpPendingRestart);
        }
    }
}
#endif
//---------------------------------------------------------------------------
void flxApplication::onSystemActivity(void)
{
#if defined(CONFIG_FLUX_APP_LED)
    theLED.flash(flxColor::Gray);
#endif
}

//---------------------------------------------------------------------------
void flxApplication::onSystemActivityLow(void)
{
#if defined(CONFIG_FLUX_APP_LED)
    theLED.flash(flxColor::Blue);
#endif
}

#if defined(CONFIG_FLUX_APP_BUTTON)
//---------------------------------------------------------------------------
// Button Events - general handler
//---------------------------------------------------------------------------
//
// CAlled when the button is pressed and an increment time passed
void flxApplication::onButtonPressed(uint32_t increment)
{

    // we need LED on for visual feedback...
    theLED.setDisabled(false);

    if (increment == 1)
#if defined(CONFIG_FLUX_APP_LED)
        theLED.blinkSlow(flxColor::Yellow);
#endif
    else if (increment == 2)
#if defined(CONFIG_FLUX_APP_LED)
        theLED.blinkMedium(flxColor::Green);
#endif
    else if (increment == 3)
#if defined(CONFIG_FLUX_APP_LED)
        theLED.blinkFast(flxColor::Blue);
#endif

    else if (increment >= 4)
    {
#if defined(CONFIG_FLUX_APP_LED)
        theLED.stop();

        theLED.on(flxColor::Red);
        delay(500);
        theLED.off();
#endif

        // Reset time !
        resetDevice();
    }
}
//---------------------------------------------------------------------------
void flxApplication::onButtonReleased(uint32_t increment)
{
#if defined(CONFIG_FLUX_APP_LED)
    if (increment > 0)
        theLED.off();
#endif
}
#endif
//---------------------------------------------------------------------------
// Flux flxApplication LifeCycle Method
//---------------------------------------------------------------------------
// onSetup()
//
// Called by the system before devices are loaded, and system initialized
bool flxApplication::sysSetup()
{

    // do we need to disable startup messages (Warn and Error still displayed)
    if (startupOutputMode() == kAppStartupMsgNone)
        flxLog.setLogLevel(flxLogWarning);

    // flxLog_I("DEBUG: onSetup() enter - Free Heap: %d", ESP.getFreeHeap());

    // Version info
    setVersion(FLUX_APP_VERSION_MAJOR, FLUX_APP_VERSION_MINOR, FLUX_APP_VERSION_POINT, FLUX_APP_VERSION_DESCRIPTOR,
               BUILD_NUMBER);

#if defined(CONFIG_FLUX_PREFS)
    // set the settings storage system for the framework
    flxSettings.setStorage(&_sysStorage);

#if defined(CONFIG_FLUX_PREFS_JSON)
    flxSettings.setFallback(&_jsonStorage);
#endif
#endif

// TODO -- this should take into account the various FS storage options in the frameowrk
#if defined(CONFIG_FLUX_SDMMCARD) && defined(CONFIG_FLUX_PREFS_JSON)
    // Have JSON storage write/use the SD card
    _jsonStorage.setFileSystem(&_theSDCard);
    _jsonStorage.setFilename("datalogger.json");
#endif

#if defined(CONFIG_FLUX_PREFS_SERIAL)
    // Have settings saved when editing via serial console is complete.
    flxRegisterEventCB(flxEvent::kOnEdit, this, &flxApplication::onSettingsEdit);
    flxRegisterEventCB(flxEvent::kOnEditFinished, &flxSettings, &flxSettingsSave::saveEvent_CB);

    // flxRegisterEventCB(flxEvent::kOnNewFile, &flxSettings, &flxSettingsSave::saveEvent_CB);

    // Add serial settings to flux - the flux loop call will take care
    // of everything else.
    flux.add(_serialSettings);
#endif

#if defined(CONFIG_FLUX_WIFI)
    _wifiConnection.setTitle("Network");
#endif

#if defined(CONFIG_FLUX_NTP)
    // wire up the NTP to the wifi network object. When the connection status changes,
    // the NTP client will start and stop.
#if defined(CONFIG_FLUX_WIFI)
    _ntpClient.setNetwork(&_wifiConnection);
#endif
    _ntpClient.setStartupDelay(kAppNTPStartupDelaySecs); // Give the NTP server some time to start

    // set our default clock to NTP - this will be overwritten if prefs are loaded
#if defined(CONFIG_FLUX_CLOCK)
    flxClock.referenceClock = _ntpClient.name();
#endif
#endif
    // Setup the IoT clients
    // if (!setupIoTClients())
    //     flxLog_W(F("Error initializing IoT Clients"));

    //----------
    // setup firmware update/reset system

#if defined(CONFIG_FLUX_FIRMWARE)
#if defined(CONFIG_FLUX_SDMMCARD)
    // Filesystem to read firmware from
    _sysUpdate.setFileSystem(&_theSDCard);
#endif

#if defined(CONFIG_FLUX_PREFS_SERIAL)
    // Serial UX - used to list files to select off the filesystem
    _sysUpdate.setSerialSettings(_serialSettings);
#endif
    _sysUpdate.setFirmwareFilePrefix(kDataLoggerFirmwareFilePrefix);

#if defined(CONFIG_FLUX_WIFI)
    _sysUpdate.setWiFiDevice(&_wifiConnection);
    _sysUpdate.enableOTAUpdates(kDataLoggerOTAManifestURL);
#endif

    flxRegisterEventCB(flxEvent::kOnFirmwareLoad, this, &flxApplication::onFirmwareLoad);

    // Add to the system - manual add so it appears last in the ops list
    _sysUpdate.setTitle("Advanced");
    flux.add(_sysUpdate);

#endif

#if defined(CONFIG_FLUX_APP_BUTTON)
    // The on-board button
    flux.add(_boardButton);

    // We want an event every 5 seconds
    _boardButton.setPressIncrement(kButtonPressedIncrement);

    // Button events we're listening on
    _boardButton.on_buttonRelease.call(this, &flxApplication::onButtonReleased);
    _boardButton.on_buttonPressed.call(this, &flxApplication::onButtonPressed);
#endif
    // wire in LED to the logging system
    // flxRegisterEventCB(flxEvent::kLogErrWarn, this, &flxApplication::onErrorMessage);
    // was device auto load disabled by startup commands?
    if (inOpMode(kFlxApplicationOpStartNoAutoload))
        flux.setAutoload(false);

    // was settings restore disabled by startup commands?
    if (inOpMode(kFlxApplicationOpStartNoSettings))
        flux.setLoadSettings(false);

#if defined(CONFIG_FLUX_WIFI)
    // was wifi startup disabled by startup commands?
    if (inOpMode(kFlxApplicationOpStartNoWiFi))
        _wifiConnection.setDelayedStartup();
#endif
    // was wifi startup disabled by startup commands?
    if (inOpMode(kFlxApplicationOpStartListDevices))
        flux.dumpDeviceAutoLoadTable();

    // setup our event callbacks for system/framework events;
    // flxRegisterEventCB(flxEvent::kOnSystemActivity, this, &flxApplication::onSystemActivity);
    // flxRegisterEventCB(flxEvent::kOnSystemActivityLow, this, &flxApplication::onSystemActivityLow);

    return flxApplicationBase::sysSetup();
}

//---------------------------------------------------------------------------
// Flux flxApplication LifeCycle Method
//---------------------------------------------------------------------------
//---------------------------------------------------------------------
// onDeviceLoad()
//
// Called after qwiic/i2c auto-load, but before system state restore

void flxApplication::sysDeviceLoad()
{

    // // setup the GNSS device - will create some properties that should be visible
    // // after the device is loaded and before restore (if the settings are saved)
    // // setupGNSS();

    // // setup the external serial device manager
    // setupExtSerial();

    // // setup interrupt event
    // setInterruptEvent();
    flxApplicationBase::sysDeviceLoad();
}
//---------------------------------------------------------------------
// onRestore()
//
// Called just before settings are restored on startup.

void flxApplication::sysRestore(void)
{
    // At this point, we know enough about the device to set details about it.
    char prefix[5] = "0000";
    // (void)dlModeCheckPrefix(_modeFlags, prefix);
    setAppClassID(FLUX_APP_CLASS_NAME_ID, prefix); // internal name string for this app type

    flxApplicationBase::sysRestore();
}

//---------------------------------------------------------------------
// reset the device - erase settings, reboot
void flxApplication::resetDevice(void)
{
#if defined(CONFIG_FLUX_PREFS)
    _sysStorage.resetStorage();
#endif

#if defined(CONFIG_FLUX_FIRMWARE)
    _sysUpdate.restartDevice();
#endif
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Flux flxApplication LifeCycle Method
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
///
/// @brief Checks for any before startup commands from the user
///

void flxApplication::onInitStartupCommands(uint delaySecs)
{

    // Waking up from sleep?
    if (boot_count > 0)
        return;

    uint32_t delayMS = delaySecs * 1000;

    uint32_t nIterations = delayMS / kStartupLoopDelayMS;

    // setup our commands
    //
    // may a simple command table
    typedef struct
    {
        char key;
        uint16_t mode;
        const char *name;
    } startupCommand_t;
    startupCommand_t commands[] = {
        {'n', kFlxApplicationOpNone, "normal-startup"},
        {'v', kAppOpStartVerboseOutput, "verbose-output-enabled"},
        {'a', kFlxApplicationOpStartNoAutoload, "device-auto-load-disabled"},
        {'l', kFlxApplicationOpStartListDevices, "i2c-driver-listing-enabled"},
#if defined(CONFIG_FLUX_WIFI)
        {'w', kFlxApplicationOpStartNoWiFi, "wifi-disabled"},
#endif
#if defined(CONFIG_FLUX_PREFS)
        {'s', kFlxApplicationOpStartNoSettings, "settings-restore-disabled"},
#endif
    };

    // Default
    int iCommand = 0;

    Serial.printf("\n\r");

    if (nIterations > 0)
    {
        // clear buffer
        while (Serial.available() > 0)
            Serial.read();

        bool menuSelected = false;
        Serial.printf("Startup Options:\n\r");
        for (int n = 0; n < sizeof(commands) / sizeof(startupCommand_t); n++)
            Serial.printf("   '%c' = %s\n\r", commands[n].key, commands[n].name);
        Serial.printf("Select Option[%2us]:", delaySecs);

        char chBS = 8; // backspace
        char chCodeBell = 7;

        for (int i = 0; i < nIterations && !menuSelected; i++)
        {
            delayMS -= kStartupLoopDelayMS;
            if (delayMS / 1000 != delaySecs)
            {
                Serial.write(chBS);
                Serial.write(chBS);
                Serial.write(chBS);
                Serial.write(chBS);
                Serial.write(chBS);
                delaySecs = delayMS / 1000;
                Serial.printf("%2us]:", delaySecs);
            }

            delay(kStartupLoopDelayMS);

            if (Serial.available() > 0)
            {
                // code /key pressed
                uint8_t chIn = Serial.read();

                // any match
                for (int n = 0; n < sizeof(commands) / sizeof(startupCommand_t); n++)
                {
                    if (chIn == commands[n].key)
                    {
                        iCommand = n;
                        menuSelected = true;
                        break;
                    }
                }
                if (!menuSelected)
                    Serial.write(chCodeBell); // bad char
            }
        }
    }
    // set the op mode
    setOpMode(commands[iCommand].mode);
    Serial.printf(" %s\n\r", commands[iCommand].name);
}
//---------------------------------------------------------------------------
// onInit()
//
// Called before the system/framework is up

void flxApplication::sysInit(void)
{
    // Did the user set a serial value?
    uint32_t theRate;
    uint32_t theDelay;
    getStartupProperties(theRate, theDelay);

    // just to be safe...
    theRate = theRate >= 1200 ? theRate : kDefaultTerminalBaudRate;

    Serial.begin(theRate);

    // Wait on serial - not sure if a timeout is needed ... but added for safety
    for (uint32_t startMS = millis(); !Serial && millis() - startMS <= kSerialStartupDelayMS;)
        delay(250);

#if defined(CONFIG_FLUX_APP_LED)
    if (!theLED.initialize(FLUX_BOARD_APP_LED))
        flxLog_W(F("LED failed to initialize"));
    theLED.on(flxColor::Green);
#endif

    setOpMode(kFlxApplicationOpStartup);

    startupDelaySecs = theDelay;
    onInitStartupCommands(theDelay);

    // #if defined(CONFIG_FLUX_CLOCK)
    //     // change the order of the system settings
    //     flux.insert_after(&flxSettings, &flxClock);
    // #endif

    // button
#if defined(FLUX_BOARD_APP_BUTTON)
    _boardButton.setButtonPin(FLUX_BOARD_APP_BUTTON);
#endif

    flxApplicationBase::sysInit();
}

//---------------------------------------------------------------------------
// Flux flxApplication LifeCycle Method
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// onStart()
//
// Called after the system is loaded, restored and initialized
bool flxApplication::sysStart()
{

    // flxLog_I("DEBUG: onStart() - entry -  Free Heap: %d", ESP.getFreeHeap());

    // Waking up from a sleep (boot count isn't zero)
    if (boot_count != 0)
    {
        flxLog_I(F("Starting system from deep sleep - boot_count %d - wake period is %d seconds"), boot_count,
                 wakeInterval());

        // Print the wakeup reason
        esp_sleep_wakeup_cause_t wakeup_reason;
        wakeup_reason = esp_sleep_get_wakeup_cause();
        switch (wakeup_reason)
        {
        case ESP_SLEEP_WAKEUP_EXT0:
            flxLog_I(F("Wakeup caused by external signal using RTC_IO"));
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            flxLog_I(F("Wakeup caused by external signal using RTC_CNTL"));
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            flxLog_I(F("Wakeup caused by timer"));
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            flxLog_I(F("Wakeup caused by touchpad"));
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            flxLog_I(F("Wakeup caused by ULP program"));
            break;
        default:
            flxLog_I(F("Wakeup was not caused by deep sleep: %d"), (int)wakeup_reason);
            break;
        }
    }

    boot_count++;

    // init wifi
    _wifiConnection.connect();
    // Logging is done at an interval - using an interval timer.
    // Connect logger to the timer event
    _logger.listen(_timer.on_interval_with_name);

    //  - Add the JSON and CVS format to the logger
    _logger.add(_fmtJSON);
    _logger.add(_fmtCSV);

    // // check SD card status
    // if (!_theSDCard.enabled())
    // {
    //     // disable SD card output
    //     set_logTypeSD(kAppLogTypeNone);
    // }

    // // setup NFC - it provides another means to load WiFi credentials
    // setupNFDevice();

    // now lets rock on the external serial device
    // if (_extSerial.begin())
    //     flxLog_I(F("External Serial Device started: RX %u, TX %u, Baud: %u"), _extSerial.rxPin(), _extSerial.txPin(),
    //              _extSerial.serialBaudRate());
    // else
    //     flxLog_I(F("External Serial Device not started"));

    flxLog_N("");

    // check our I2C devices
    // Loop over the device list - note that it is iterable.
    flxLog_I_(F("Loading devices ... "));
    flxDeviceContainer loadedDevices = flux.connectedDevices();

    if (loadedDevices.size() == 0)
        flxLog_N(F("no devices detected"));
    else
    {
        flxLog_N(F("%d devices detected"), loadedDevices.size());
        for (auto device : loadedDevices)
        {
            // output the connected devices ... include device type/address
            flxLog_N_(F("    %-20s  - %-40s  {"), device->name(), device->description());
            if (device->getKind() == flxDeviceKindI2C)
                flxLog_N("%s x%x}", "qwiic", device->address());
            else if (device->getKind() == flxDeviceKindSPI)
                flxLog_N("%s p%u}", "SPI", device->address());
            else if (device->getKind() == flxDeviceKindGPIO)
                flxLog_N("%s p%u}", "GPIO", device->address());

            if (device->nOutputParameters() > 0)
                _logger.add(device);
        }
    }

    // Setup the Bio Hub
    // setupBioHub();

    // setup the ENS160
    // setupENS160();

    // Check time devices
    if (!setupTime())
        flxLog_W(F("Time reference setup failed."));

    flxLog_N("");

    // set our system start time in milliseconds
    _startTime = millis();

    if (startupOutputMode() == kAppStartupMsgNormal)
        displayAppStatus(true);

    // if (!_isValidMode)
    //     outputVMessage();

    // // for our web server file search
    // _iotWebServer.setFilePrefix(_theOutputFile.filePrefix());

    // Register our device management event handlers
    flxRegisterEventCB(flxEvent::kOnFluxAddDevice, this, &flxApplication::onDeviceAdded);
    flxRegisterEventCB(flxEvent::kOnFluxRemoveDevice, this, &flxApplication::onDeviceRemoved);

    // clear startup flags/mode
    clearOpMode(kFlxApplicationOpStartup);
    clearOpMode(kFlxApplicationOpStartAllFlags);

#if defined(CONFIG_FLUX_APP_LED)
    theLED.off();
#endif
    // call the app base -- which will call out to the user onStart method
    bool rc = flxApplicationBase::sysStart();
    // we are done with startup - reset output mode
    if (startupOutputMode() != kAppStartupMsgNormal)
        flxLog.setLogLevel(flxLogInfo);

    // log now!
    _timer.trigger();
    return rc;
}

//---------------------------------------------------------------------------
void flxApplication::enterSleepMode()
{

    if (!sleepEnabled())
        return;

    time_t t_now;
    time(&t_now);
    struct tm *tmLocal = localtime(&t_now);
    char szBuffer[32];
    strftime(szBuffer, sizeof(szBuffer), "%d-%m-%G %T", tmLocal);

    flxLog_I(F("%s: Starting device deep sleep for %u secs"), szBuffer, sleepInterval());

    // esp_sleep_config_gpio_isolate(); // Don't. This causes: E (33643) gpio: gpio_sleep_set_pull_mode(827): GPIO
    // number error
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF); // Don't disable RTC SLOW MEM - otherwise
    // boot_count (RTC_DATA_ATTR) becomes garbage
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF);

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

    unsigned long long period = sleepInterval() * 1000000ULL;

    esp_sleep_enable_timer_wakeup(period);

    esp_deep_sleep_start(); // see you on the other side
}

// simple helper to get the build time of the firmware
const char *flxApplication::getBuildDate(void)
{
    return __TIMESTAMP__;
}

//---------------------------------------------------------------------------
// Device bookkeeping
//---------------------------------------------------------------------------
void flxApplication::onDeviceAdded(uint32_t uiDevice)
{
    // if in startup skip
    if (inOpMode(kFlxApplicationOpStartup))
        return;

    flxDevice *pDevice = (flxDevice *)uiDevice;
    if (pDevice == nullptr)
        return;

#if defined(CONFIG_FLUX_LOGGING)
    // add this device to the logger
    _logger.add(pDevice);
#endif
}
void flxApplication::onDeviceRemoved(uint32_t uiDevice)
{
    // if in startup skip
    if (inOpMode(kFlxApplicationOpStartup))
        return;

    flxDevice *pDevice = (flxDevice *)uiDevice;
    if (pDevice == nullptr)
        return;

#if defined(CONFIG_FLUX_LOGGING)
    // remove this device from the logger
    _logger.remove(pDevice);
#endif
}
//---------------------------------------------------------------------------
// loop()
//
// Called during the operational loop of the system.

bool flxApplication::loop()
{
    // key press at Serial Console? What to do??
    if (Serial.available())
    {
#if defined(CONFIG_FLUX_APP_COMMANDS)
        // Bang command?
        uint8_t chIn = Serial.read();
        if (chIn == '!')
        {
            flxSerial.textToWhite();
            Serial.write('>');
            flxSerial.textToNormal();
            Serial.write('!');
            Serial.flush();
            flxAppCommands cmdProcessor;
            bool status = cmdProcessor.processCommand(this);
        }
        else // edit settings
        {
#endif
            // start an editing session
            flxColor::color color;
            int status = _serialSettings.editSettings();
            if (status == -1)
                color = flxColor::Red;
            else if (status == 1)
                color = flxColor::Green;
            else
                color = flxColor::Yellow;
            theLED.flash(color);
#if defined(CONFIG_FLUX_APP_COMMANDS)
        }
#endif
    }
    return false;
}

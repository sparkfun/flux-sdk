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
 
/*
 * Spark Framework demo - logging
 *
 */

// Spark framework
#include <Flux.h>
#include <Flux/flxCoreLog.h>
#include <Flux/flxFmtCSV.h>
#include <Flux/flxFmtJSON.h>
#include <Flux/flxLogger.h>
#include <Flux/flxSerial.h>
#include <Flux/flxTimer.h>

// settings storage
#include <Flux/flxSettings.h>
#include <Flux/flxSettingsSerial.h>
#include <Flux/flxStorageESP32Pref.h>
#include <Flux/flxStorageJSONPref.h>

// SD Card output
#include <Flux/flxFSSDMMCard.h>
#include <Flux/flxFileRotate.h>

// WiFi and NTP
#include <Flux/flxWiFiESP32.h>
#include <Flux/flxNTPESP32.h>

// NFC device
#include <Flux/flxDevST25DV.h>

// SPI Devices
// The onboard IMU
#include <Flux/flxDevISM330.h>
static const uint8_t kAppOnBoardIMUCS = 5;

// The onboard Magnetometer
#include <Flux/flxDevMMC5983.h>
static const uint8_t kAppOnBoardMAGCS = 27;

// Biometric Hub -- requires pins to be set on startup
#include <Flux/flxDevBioHub.h>
static const uint8_t kAppBioHubReset = 17; // Use the TXD pin as the bio hub reset pin
static const uint8_t kAppBioHubMFIO = 16;  // Use the RXD pin as the bio hub mfio pin

// IoT Client Icludes
#include <Flux/flxIoTAWS.h>
#include <Flux/flxIoTAzure.h>
#include <Flux/flxMQTTESP32.h>
#include <Flux/flxIoTThingSpeak.h>
#include <Flux/flxIoTHTTP.h>

//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 15000

// Buffersize of our JSON document output
#define kAppJSONDocSize 1400

// NTP Startup delay in secs

#define kAppNTPStartupDelaySecs 5



/////////////////////////////////////////////////////////////////////////
// Define our application class for the data logger
/////////////////////////////////////////////////////////////////////////
class sfeDataLogger : public flxApplication
{
private:
    //---------------------------------------------------------------------------
    // setupSDCard()
    //
    // Set's up the SD card subsystem and the objects/systems that use it.
    bool setupSDCard(void);

    //---------------------------------------------------------------------
    // Setup the IOT clients
    bool setupIoTClients(void);

  public:
    //---------------------------------------------------------------------------
    // Constructor
    //

    sfeDataLogger();

    //---------------------------------------------------------------------------
    // setup()
    //
    // Called by the system before devices are loaded, and system initialized
    bool setup();

  private:
    //---------------------------------------------------------------------
    // Check if we have a NFC reader available -- for use with WiFi credentials
    //
    // Call after autoload
    void setupNFDevice(void);

    //---------------------------------------------------------------------
    void setupSPIDevices(void);
    
    //---------------------------------------------------------------------
    void setupBioHub(void);
   
    //------------------------------------------
    // For controlling the log output types

    static constexpr uint8_t kAppLogTypeNone  =   0x0;
    static constexpr uint8_t kAppLogTypeCSV   =   0x1;
    static constexpr uint8_t kAppLogTypeJSON  =   0x2;


    //---------------------------------------------------------------------------
    uint8_t get_logTypeSD(void);
    
    //---------------------------------------------------------------------------
    void set_logTypeSD(uint8_t logType);
    
    //---------------------------------------------------------------------------
    uint8_t get_logTypeSer(void);
    
    //---------------------------------------------------------------------------
    void set_logTypeSer(uint8_t logType);
    
    uint8_t    _logTypeSD;
    uint8_t    _logTypeSer;    

  public:
    //---------------------------------------------------------------------------
    // start()
    //
    // Called after the system is loaded, restored and initialized
    bool start();

    bool loop();

    // Define our log type properties

    flxPropertyRWUint8<sfeDataLogger, &sfeDataLogger::get_logTypeSD, &sfeDataLogger::set_logTypeSD>   
                sdCardLogType = { kAppLogTypeCSV, { {"Disabled", kAppLogTypeNone},
                                                    {"CSV Format", kAppLogTypeCSV},
                                                    {"JSON Format", kAppLogTypeJSON} } };

    flxPropertyRWUint8<sfeDataLogger, &sfeDataLogger::get_logTypeSer, &sfeDataLogger::set_logTypeSer> 
                serialLogType = { kAppLogTypeCSV, { {"Disabled", kAppLogTypeNone},
                                                    {"CSV Format", kAppLogTypeCSV},
                                                    {"JSON Format", kAppLogTypeJSON} } };

    // System sleep properties
    flxPropertyInt<sfeDataLogger>   sleepInterval = {5, 86400};
    flxPropertyInt<sfeDataLogger>   wakeInterval = {60, 86400};    
    flxPropertyBool<sfeDataLogger>  sleepEnabled = {true};

  private:

    void enterSleepMode(void);

    // Class members -- that make up the apllication structure

    // Create a JSON and CSV output formatters.
    // Note: setting internal buffer sizes using template to minimize alloc calls.
    flxFormatJSON<kAppJSONDocSize> _fmtJSON;
    flxFormatCSV _fmtCSV;

    // Our logger
    flxLogger _logger;

    // Timer for event logging
    flxTimer _timer;

    // SD Card Filesystem object
    flxFSSDMMCard _theSDCard;

    // A writer interface for the SD Card that also rotates files
    flxFileRotate _theOutputFile;

    // settings things
    flxStorageESP32Pref _sysStorage;
    flxSettingsSerial _serialSettings;
    flxStorageJSONPref _jsonStorage;

    // WiFi and NTP
    flxWiFiESP32 _wifiConnection;
    flxNTPESP32 _ntpClient;

    // the onboard IMU
    flxDevISM330_SPI _onboardIMU;
    flxDevMMC5983_SPI _onboardMag;

    // a biometric sensor hub
    flxDevBioHub _bioHub;

    // IoT endpoints
    // An generic MQTT client
    flxMQTTESP32 _mqttClient;

    // AWS
    flxIoTAWS _iotAWS;

    // Thingspeak
    flxIoTThingSpeak _iotThingSpeak;

    // azure
    flxIoTAzure _iotAzure;

    // HTTP/URL Post
    flxIoTHTTP _iotHTTP;

    // For the sleep timer
    unsigned long _startTime = 0;
};

/*
 * Spark Framework demo - logging
 *
 */

// Spark framework
#include <Spark.h>
#include <Spark/spFmtCSV.h>
#include <Spark/spFmtJSON.h>
#include <Spark/spLogger.h>
#include <Spark/spSerial.h>
#include <Spark/spTimer.h>

// settings storage
#include <Spark/spSettings.h>
#include <Spark/spSettingsSerial.h>
#include <Spark/flxStorageESP32Pref.h>
#include <Spark/flxStorageJSONPref.h>

// SD Card output
#include <Spark/spFSSDMMCard.h>
#include <Spark/spFileRotate.h>

// WiFi and NTP
#include <Spark/spWiFiESP32.h>
#include <Spark/spNTPESP32.h>

// NFC device
#include <Spark/spDevsT25DV.h>

// SPI Devices
// The onboard IMU
#include <Spark/spDevISM330.h>
static const uint8_t kAppOnBoardIMUCS = 5;

// The onboard Magnetometer
#include <Spark/spDevMMC5983.h>
static const uint8_t kAppOnBoardMAGCS = 27;

// Biometric Hub -- requires pins to be set on startup
#include <Spark/spDevBioHub.h>
static const uint8_t kAppBioHubReset = 17; // Use the TXD pin as the bio hub reset pin
static const uint8_t kAppBioHubMFIO = 16;  // Use the RXD pin as the bio hub mfio pin

// IoT Client Icludes
#include <Spark/spAWSIoT.h>
#include <Spark/spAzureIoT.h>
#include <Spark/spMQTTESP32.h>
#include <Spark/spThingSpeak.h>
#include <Spark/spWrHTTP.h>

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
class sfeDataLogger : public spApplication
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

    // Define our log type properties

    spPropertyRWUint8<sfeDataLogger, &sfeDataLogger::get_logTypeSD, &sfeDataLogger::set_logTypeSD>   
                sdCardLogType = { kAppLogTypeCSV, { {"Disabled", kAppLogTypeNone},
                                                    {"CSV Format", kAppLogTypeCSV},
                                                    {"JSON Format", kAppLogTypeJSON} } };

    spPropertyRWUint8<sfeDataLogger, &sfeDataLogger::get_logTypeSer, &sfeDataLogger::set_logTypeSer> 
                serialLogType = { kAppLogTypeCSV, { {"Disabled", kAppLogTypeNone},
                                                    {"CSV Format", kAppLogTypeCSV},
                                                    {"JSON Format", kAppLogTypeJSON} } };

  private:
    // Class members -- that make up the apllication structure

    // Create a JSON and CSV output formatters.
    // Note: setting internal buffer sizes using template to minimize alloc calls.
    spFormatJSON<kAppJSONDocSize> _fmtJSON;
    spFormatCSV _fmtCSV;

    // Our logger
    spLogger _logger;

    // Timer for event logging
    spTimer _timer;

    // SD Card Filesystem object
    spFSSDMMCard _theSDCard;

    // A writer interface for the SD Card that also rotates files
    spFileRotate _theOutputFile;

    // settings things
    flxStorageESP32Pref _sysStorage;
    spSettingsSerial _serialSettings;
    flxStorageJSONPref _jsonStorage;

    // WiFi and NTP
    spWiFiESP32 _wifiConnection;
    spNTPESP32 _ntpClient;

    // the onboard IMU
    spDevISM330_SPI _onboardIMU;
    spDevMMC5983_SPI _onboardMag;

    // a biometric sensor hub
    spDevBioHub _bioHub;

    // IoT endpoints
    // An generic MQTT client
    spMQTTESP32 _mqttClient;

    // AWS
    spAWSIoT _iotAWS;

    // Thingspeak
    spThingSpeak _iotThingSpeak;

    // azure
    spAzureIoT _iotAzure;

    // HTTP/URL Post
    spHTTPIoT _iotHTTP;
};

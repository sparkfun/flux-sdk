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
#include <Spark/spStorageESP32Pref.h>
#include <Spark/spStorageJSONPref.h>

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
    bool setupSDCard(void)
    {
        // setup output to the SD card
        if (_theSDCard.initialize())
        {

            _theOutputFile.setName("Data File", "Output file rotation manager");

            // SD card is available - lets setup output for it
            // Add the filesystem to the file output/rotation object
            _theOutputFile.setFileSystem(_theSDCard);

            // setup our file rotation parameters
            _theOutputFile.filePrefix = "sfe";
            _theOutputFile.startNumber = 1;
            _theOutputFile.rotatePeriod(24); // one day

            // add the file output to the CSV output.
            _fmtCSV.add(_theOutputFile);

            // have the CSV formatter listen to the new file event. This
            // will cause a header to be written next cycle.
            _fmtCSV.listenNewFile(_theOutputFile.on_newFile);

            return true;
        }
        return false;
    }

    //---------------------------------------------------------------------
    // Setup the IOT clients
    bool setupIoTClients()
    {

        // setup the network connection for the mqtt
        _mqttClient.setNetwork(&_wifiConnection);
        // add mqtt to JSON
        _fmtJSON.add(_mqttClient);

        // AWS
        _iotAWS.setName("AWS IoT", "Connect to an AWS Iot Thing");
        _iotAWS.setNetwork(&_wifiConnection);

        // Add the filesystem to load certs/keys from the SD card
        _iotAWS.setFileSystem(&_theSDCard);
        _fmtJSON.add(_iotAWS);

        // Thingspeak driver
        _iotThingSpeak.setNetwork(&_wifiConnection);

        // Add the filesystem to load certs/keys from the SD card
        _iotThingSpeak.setFileSystem(&_theSDCard);
        _fmtJSON.add(_iotThingSpeak);

        // Azure IoT
        _iotAzure.setNetwork(&_wifiConnection);

        // Add the filesystem to load certs/keys from the SD card
        _iotAzure.setFileSystem(&_theSDCard);
        _fmtJSON.add(_iotAzure);

        // general HTTP / URL logger

        _iotHTTP.setNetwork(&_wifiConnection);
        _iotHTTP.setFileSystem(&_theSDCard);
        _fmtJSON.add(_iotHTTP);

        return true;
    }

  public:
    //---------------------------------------------------------------------------
    // Constructor
    //

    sfeDataLogger() : _timer{kDefaultLogInterval}
    {
        // nothing
    }

    //---------------------------------------------------------------------------
    // setup()
    //
    // Called by the system before devices are loaded, and system initialized
    bool setup()
    {
        // Lets set the application name?!
        setName("SparkFun Data Logger ESP32", "(c) 2023 SparkFun Electronics");
        setVersion("0.9.1 Alpha", 10009001);

        // set the settings storage system for spark
        spSettings.setStorage(&_sysStorage);
        spSettings.setFallback(&_jsonStorage);

        // Have JSON storage write/use the SD card
        _jsonStorage.setFileSystem(&_theSDCard);
        _jsonStorage.setFilename("openlog.json");

        // Have settings saved when editing via serial console is complete.
        spSettings.listenForSave(_serialSettings.on_finished);

        // Add serial settings to spark - the spark loop call will take care
        // of everything else.
        spark.add(_serialSettings);

        // wire up the NTP to the wifi network object. When the connection status changes,
        // the NTP client will start and stop.
        _ntpClient.setNetwork(&_wifiConnection);
        _ntpClient.setStartupDelay(kAppNTPStartupDelaySecs); // Give the NTP server some time to start

        // setup SD card. Do this before calling start - so prefs can be read off SD if needed
        if (!setupSDCard())
            spLog_W(F("Error initializing the SD Card"));

        // Setup the IoT clients
        if (!setupIoTClients())
            spLog_W(F("Error initializing IoT Clients"));

        return true;
    }

  private:
    //---------------------------------------------------------------------
    // Check if we have a NFC reader available -- for use with WiFi credentials
    //
    // Call after autoload
    void setupNFDevice(void)
    {
        // do we have a NFC device connected?
        auto nfcDevices = spark.get<spDevST25DV>();

        if (nfcDevices->size() == 0)
            return;

        // We have an NFC device. Create a credentials action and connect to the NFC device
        // and WiFi.
        spSetWifiCredentials *pCreds = new spSetWifiCredentials;

        if (!pCreds)
            return;

        spDevST25DV *pNFC = nfcDevices->at(0);
        spLog_I(F("%s: WiFi credentials via NFC enabled"), pNFC->name());

        pCreds->setCredentialSource(pNFC);
        pCreds->setWiFiDevice(&_wifiConnection);

        // Change the name on the action
        pCreds->setName("WiFi Login From NFC", "Set the devices WiFi Credentials from an attached NFC source.");
    }

    //---------------------------------------------------------------------
    void setupSPIDevices()
    {
        // Note - framework is setting up the pins ...
        // IMU
        if (_onboardIMU.initialize(kAppOnBoardIMUCS))
        {
            spLog_I(F("Onboard %s is enabled"), _onboardIMU.name());
            _logger.add(_onboardIMU);
        }
        else
            spLog_E(F("Onboard %s failed to start"), _onboardIMU.name());

        // Magnetometer
        if (_onboardMag.initialize(kAppOnBoardMAGCS))
        {
            spLog_I(F("Onboard %s is enabled"), _onboardMag.name());
            _logger.add(_onboardMag);
        }
        else
            spLog_E(F("Onboard %s failed to start"), _onboardMag.name());
    }
    //---------------------------------------------------------------------
    void setupBioHub()
    {
        if (_bioHub.initialize(kAppBioHubReset,
                               kAppBioHubMFIO)) // Initialize the bio hub using the reset and mfio pins,
        {
            spLog_I(F("%s is enabled"), _bioHub.name());
            _logger.add(_bioHub);
        }
    }

  public:
    //---------------------------------------------------------------------------
    // start()
    //
    // Called after the system is loaded, restored and initialized
    bool start()
    {
        // printout the device ID
        spLog_I(F("Device ID: %s"), spark.deviceId());

        // Write out the SD card stats
        if (_theSDCard.enabled())
            spLog_I(F("SD card available. Type: %s, Size: %uMB"), _theSDCard.type(), _theSDCard.size());
        else
            spLog_W(F("SD card not available."));

        // WiFi status
        if (!_wifiConnection.isConnected())
            spLog_E(F("Unable to connect to WiFi!"));

        // Logging is done at an interval - using an interval timer.
        // Connect logger to the timer event
        _logger.listen(_timer.on_interval);

        // We want to output JSON and CSV to the serial consol.
        //  - Add Serial to our  formatters
        _fmtJSON.add(spSerial());
        _fmtCSV.add(spSerial());

        //  - Add the JSON and CVS format to the logger
        _logger.add(_fmtJSON);
        _logger.add(_fmtCSV);

        // setup NFC - it provides another means to load WiFi creditials
        setupNFDevice();

        // What devices has the system detected?
        // List them and add them to the logger

        spDeviceContainer myDevices = spark.connectedDevices();

        // The device list can be added directly to the logger object using an
        // add() method call. This will only add devices with output parameters.
        //
        // Example:
        //      logger.add(myDevices);
        //
        // But for this app, let's loop over our devices and show how use the
        // device parameters.

        spLog_I(F("Devices Detected [%d]"), myDevices.size());

        // Loop over the device list - note that it is iterable.
        for (auto device : myDevices)
        {
            spLog_N_(F("\tDevice: %s, Output Number: %d\n\r"), device->name(), device->nOutputParameters());
            if (device->nOutputParameters() > 0)
                _logger.add(device);
        }

        // Setup the Onboard IMU
        setupSPIDevices();

        // Setup the Bio Hub
        setupBioHub();

        spLog_N("");
        
        return true;
    }

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
    spStorageESP32Pref _sysStorage;
    spSettingsSerial _serialSettings;
    spStorageJSONPref _jsonStorage;

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

/*
 * SparkFun Data Logger
 *
 */

#include "sfeDataLogger.h"

//---------------------------------------------------------------------------
// Constructor
//

sfeDataLogger::sfeDataLogger() : _logTypeSD{kAppLogTypeNone}, _logTypeSer{kAppLogTypeNone}, _timer{kDefaultLogInterval}
{
    spRegister(sdCardLogType, "SD Card Format", "Enable and set the output format");
    spRegister(serialLogType, "Serial Console Format", "Enable and set the output format");

    // Add the format changing props to the logger - makese more sense from a UX standpont.
    _logger.addProperty(sdCardLogType);
    _logger.addProperty(serialLogType);
}

//---------------------------------------------------------------------------
// setupSDCard()
//
// Set's up the SD card subsystem and the objects/systems that use it.
bool sfeDataLogger::setupSDCard(void)
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
        //_fmtCSV.add(_theOutputFile);

        // have the CSV formatter listen to the new file event. This
        // will cause a header to be written next cycle.
        _fmtCSV.listenNewFile(_theOutputFile.on_newFile);

        return true;
    }
    return false;
}

//---------------------------------------------------------------------
// Setup the IOT clients
bool sfeDataLogger::setupIoTClients()
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

//---------------------------------------------------------------------------
// setup()
//
// Called by the system before devices are loaded, and system initialized
bool sfeDataLogger::setup()
{
    // Lets set the application name?!
    setName("SparkFun Data Logger ESP32", "(c) 2023 SparkFun Electronics");
    setVersion("0.9.1 Alpha", 10009001);

    // set the settings storage system for spark
    flxSettings.setStorage(&_sysStorage);
    flxSettings.setFallback(&_jsonStorage);

    // Have JSON storage write/use the SD card
    _jsonStorage.setFileSystem(&_theSDCard);
    _jsonStorage.setFilename("openlog.json");

    // Have settings saved when editing via serial console is complete.
    flxSettings.listenForSave(_serialSettings.on_finished);

    // Add serial settings to spark - the spark loop call will take care
    // of everything else.
    spark.add(_serialSettings);

    // wire up the NTP to the wifi network object. When the connection status changes,
    // the NTP client will start and stop.
    _ntpClient.setNetwork(&_wifiConnection);
    _ntpClient.setStartupDelay(kAppNTPStartupDelaySecs); // Give the NTP server some time to start

    // setup SD card. Do this before calling start - so prefs can be read off SD if needed
    if (!setupSDCard())
        flxLog_W(F("Error initializing the SD Card"));

    // Setup the IoT clients
    if (!setupIoTClients())
        flxLog_W(F("Error initializing IoT Clients"));

    return true;
}

//---------------------------------------------------------------------
// Check if we have a NFC reader available -- for use with WiFi credentials
//
// Call after autoload
void sfeDataLogger::setupNFDevice(void)
{
    // do we have a NFC device connected?
    auto nfcDevices = spark.get<spDevST25DV>();

    if (nfcDevices->size() == 0)
        return;

    // We have an NFC device. Create a credentials action and connect to the NFC device
    // and WiFi.
    flxSetWifiCredentials *pCreds = new flxSetWifiCredentials;

    if (!pCreds)
        return;

    spDevST25DV *pNFC = nfcDevices->at(0);
    flxLog_I(F("%s: WiFi credentials via NFC enabled"), pNFC->name());

    pCreds->setCredentialSource(pNFC);
    pCreds->setWiFiDevice(&_wifiConnection);

    // Change the name on the action
    pCreds->setName("WiFi Login From NFC", "Set the devices WiFi Credentials from an attached NFC source.");
}

//---------------------------------------------------------------------
void sfeDataLogger::setupSPIDevices()
{
    // Note - framework is setting up the pins ...
    // IMU
    if (_onboardIMU.initialize(kAppOnBoardIMUCS))
    {
        flxLog_I(F("Onboard %s is enabled"), _onboardIMU.name());
        _logger.add(_onboardIMU);
    }
    else
        flxLog_E(F("Onboard %s failed to start"), _onboardIMU.name());

    // Magnetometer
    if (_onboardMag.initialize(kAppOnBoardMAGCS))
    {
        flxLog_I(F("Onboard %s is enabled"), _onboardMag.name());
        _logger.add(_onboardMag);
    }
    else
        flxLog_E(F("Onboard %s failed to start"), _onboardMag.name());
}
//---------------------------------------------------------------------
void sfeDataLogger::setupBioHub()
{
    if (_bioHub.initialize(kAppBioHubReset,
                           kAppBioHubMFIO)) // Initialize the bio hub using the reset and mfio pins,
    {
        flxLog_I(F("%s is enabled"), _bioHub.name());
        _logger.add(_bioHub);
    }
}

//---------------------------------------------------------------------------
uint8_t sfeDataLogger::get_logTypeSD(void)
{
    return _logTypeSD;
}
//---------------------------------------------------------------------------
void sfeDataLogger::set_logTypeSD(uint8_t logType)
{
    if (logType == _logTypeSD)
        return;

    if (_logTypeSD == kAppLogTypeCSV)
        _fmtCSV.remove(&_theOutputFile);
    else if (_logTypeSD == kAppLogTypeJSON)
        _fmtJSON.remove(&_theOutputFile);

    _logTypeSD = logType;

    if (_logTypeSD == kAppLogTypeCSV)
        _fmtCSV.add(&_theOutputFile);
    else if (_logTypeSD == kAppLogTypeJSON)
        _fmtJSON.add(&_theOutputFile);
}
//---------------------------------------------------------------------------
uint8_t sfeDataLogger::get_logTypeSer(void)
{
    return _logTypeSer;
}
//---------------------------------------------------------------------------
void sfeDataLogger::set_logTypeSer(uint8_t logType)
{
    if (logType == _logTypeSer)
        return;

    if (_logTypeSer == kAppLogTypeCSV)
        _fmtCSV.remove(flxSerial());
    else if (_logTypeSer == kAppLogTypeJSON)
        _fmtJSON.remove(flxSerial());

    _logTypeSer = logType;

    if (_logTypeSer == kAppLogTypeCSV)
        _fmtCSV.add(flxSerial());
    else if (_logTypeSer == kAppLogTypeJSON)
        _fmtJSON.add(flxSerial());
}

//---------------------------------------------------------------------------
// start()
//
// Called after the system is loaded, restored and initialized
bool sfeDataLogger::start()
{
    // printout the device ID
    flxLog_I(F("Device ID: %s"), spark.deviceId());

    // Write out the SD card stats
    if (_theSDCard.enabled())
        flxLog_I(F("SD card available. Type: %s, Size: %uMB"), _theSDCard.type(), _theSDCard.size());
    else
        flxLog_W(F("SD card not available."));

    // WiFi status
    if (!_wifiConnection.isConnected())
        flxLog_E(F("Unable to connect to WiFi!"));

    // Logging is done at an interval - using an interval timer.
    // Connect logger to the timer event
    _logger.listen(_timer.on_interval);

    // We want to output JSON and CSV to the serial consol.
    //  - Add Serial to our  formatters
    //_fmtJSON.add(flxSerial());
    //_fmtCSV.add(flxSerial());

    //  - Add the JSON and CVS format to the logger
    _logger.add(_fmtJSON);
    _logger.add(_fmtCSV);

    // setup NFC - it provides another means to load WiFi creditials
    setupNFDevice();

    // What devices has the system detected?
    // List them and add them to the logger

    flxDeviceContainer myDevices = spark.connectedDevices();

    // The device list can be added directly to the logger object using an
    // add() method call. This will only add devices with output parameters.
    //
    // Example:
    //      logger.add(myDevices);
    //
    // But for this app, let's loop over our devices and show how use the
    // device parameters.

    flxLog_I(F("Devices Detected [%d]"), myDevices.size());

    // Loop over the device list - note that it is iterable.
    for (auto device : myDevices)
    {
        flxLog_N_(F("\tDevice: %s, Output Number: %d\n\r"), device->name(), device->nOutputParameters());
        if (device->nOutputParameters() > 0)
            _logger.add(device);
    }

    // Setup the Onboard IMU
    setupSPIDevices();

    // Setup the Bio Hub
    setupBioHub();

    flxLog_N("");

    return true;
}

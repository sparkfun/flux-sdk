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

#pragma once

#include "flxFlux.h"
#include "flxNetwork.h"

#include "flxFmtJSON.h"
#include "flxUtils.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include <ArduinoIoTCloud.h>

// ArduinoIoTCloud header defines an addProperty() macro, which of course conflicts with the
// framework - so undef it here ... VERY annoying
#undef addProperty(v, ...)

#include <map>

//----------------------------------------------------------------------------
// 10/2023 -KDB
//
// A *hack* for the ArduinoIOT system....
//
// The ArduinoIoT Cloud system uses something called a Connection Handler. This
// encapsulates the network management.
//
// For WiFi, this assumes it's controlling the connection. But this isn't the case,
// Flux is. So, create our own Connection handler and pass that into the system.
//
// This works, but would need updating if another type of connection was created for this
// system.

#include <WiFi.h>
#include <WiFiUdp.h>

/// our version of the connection handler for arduino IoT. The default
/// assumes it's controlling the WiFi. Sorry, the DataLogger is..

class DataLoggerAIOTConnectionHandler : public ConnectionHandler
{
  public:
    DataLoggerAIOTConnectionHandler(void) : ConnectionHandler{true, NetworkAdapter::WIFI}, _isConnected{false} {};

    virtual unsigned long getTime()
    {
        return 0;
    }

    virtual Client &getClient() override
    {
        return _wifi_client;
    }
    virtual UDP &getUDP() override
    {
        return _wifi_udp;
    }

  protected:
    NetworkConnectionState update_handleInit()
    {
        return (_isConnected ? NetworkConnectionState::CONNECTED : NetworkConnectionState::CONNECTING);
    }

    NetworkConnectionState update_handleConnecting()
    {
        return (_isConnected ? NetworkConnectionState::CONNECTED : NetworkConnectionState::CONNECTING);
    }

    NetworkConnectionState update_handleConnected()
    {
        return (_isConnected ? NetworkConnectionState::CONNECTED : NetworkConnectionState::DISCONNECTED);
    }
    NetworkConnectionState update_handleDisconnecting()
    {
        return NetworkConnectionState::DISCONNECTED;
    }
    NetworkConnectionState update_handleDisconnected()
    {
        return NetworkConnectionState::CLOSED;
    }

  public:
    // use this method to communicate if WiFi is up or not to this class/interface from the below class
    void setConnected(bool isConnected)
    {
        _isConnected = isConnected;
    }

  private:
    bool _isConnected;

    WiFiUDP _wifi_udp;
    WiFiClient _wifi_client;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implements a connection to the Arduino IoT Cloud
//
// Arduino IoT Cloud
//
//   The Arduino IoT Cloud is based around the concept of a "Device", which has a "Thing" associated
//   with it. The Thing is a logical container that contains "Variables" (in the GUI) aka "Properties"
//   in the Web Services API or the ArduinoCloud library.
//
//   The intent of this structure is to enable changing behavior of a device by just changing its Thing.
//
// The Flux Driver for Arduino IoT Cloud
//
//   Overall this driver does the following:
//
//      - Connects to a Thing - potentially creating the thing if needed
//      - Maps Data Parameters from the DataLogger observation to Variables in the Arduino Thing
//      - For each write() interation - the variable values are updated.
//
// Technical Details
//
//   There are two methods used to communicate with the Arduino IoT Cloud: Web Service API and a
//   mqtt based system that is obscured via the Arduino library - ArduinoIoTCloud.
//
//   Examples were provided by Arduino on how to use both these systems to implement this driver.
//
//   Web Service API
//      - Used to get a bearer (oauth) token, which is needed for the API. The user provides API credentials
//        to support this
//      - Connect to, or create a Thing for a user provided device (device ID)
//      - Create Variables/Parameters in the Thing - that will represent values from the datalogger.
//
//   ArduinoIoTCloud Library (mqtt)
//      This library is used to send value updates to the Cloud Device/Thing parameters. To do this,
//      the following is done:
//          - A local "Cloud Variable" object is created and connected to the variable in the
//            actual cloud. This driver maps this variable object to a hash id for the DataLogger
//            parameter.
//          - When values are updated, the value of the local Cloud Variable object is updated.
//          - Updates are sent to the cloud via a mqtt session - via the ArduinoCloud library.
//
//   Notes:
//      - To make this work, it's found that the Web API calls should occur before the mqtt
//        system in the ArduinoIoTCloud library is up and running. The web api calls often
//        fail once the mqtt system is running.
//
//        It appears that is is caused by the way the ArduinoIotCloud library was implemented.
//        The library is setup to manage all network connectivity ...etc, but the DataLogger is doing
//        this. It was worked around, but it's possible some aspect of this was missed.
//
//      - On startup/first use of this driver, the web API is used to get a oauth token, create/validate
//        a Thing and then create or validate Parameters in the Thing that map to datalogger data values
//
//      - Parameters from the datalogger have a hash created from their name, and this has is used to
//        map the datalogger data value to a cloud variable.
//
//      - Once up and running, the system just uses the ArduinoIoTCloud library (mqtt) to update the values
//        in the cloud.
//
//      - Once the mqtt / ArduinoIoTCloud system is up and running, and a new variable needs to be created,
//        this driver will try to create it - but will more often fail for unknow reasons. ** It appears
//        that the network connection cycles.
//
//  User requirements
//
//      At a minimum, the user needs to provide the drive the following:
//          - API Client ID
//          - API Secret
//          - Device ID
//          - Device Secret
//
//      Optional
//          - Thing Name
//          - Thing ID
//
//      Note
//          If a thing exists, but only it's name is provided, the driver needs to get the ThingID
//          using the ArduinoIoTCloud library. In this case, the creation of any additional variables
//          in the cloud (via the Web SDK) fails. It's annoying and sloppy. Best solution is to have
//          the use provide everything for the Thing (ID and Name), or just reboot the device (which
//          saves the ID) which will then have the ID at startup.
//
//  Variable/Output Parameter Map
//
//      The variable / output parameter name is mapped to a local ArduinoIot Cloud variable
//      These variables have a limited type set, and are actually objects to dynamically detect
//      when values are changed. This enables value updates to the cloud from the device/thing.
//
//      To map this the following is done:
//          - Flux Output parameter names are <device name>_<parameter name>
//          - The names are truncated to 64, left justified if needed when creating cloud vars
//          - The name used in our map object is the hash of the full name
//          - When a variable is created in the cloud, the local var is allocated and added to the map
//          - The map value is a struct that contains a type code (flxDataType_t) and a void * pointer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @typedef flxIoTArduinoVar_t
/// @brief Struct to hold a dynamically created Arduino Cloud variable
///
typedef struct
{
    flxDataType_t type;
    void *variable;
} flxIoTArduinoVar_t;

// The following helps manage our loop updates.
// Define the delta between arduino IoT cloud update calls during setup.
#define kArduinoIoTUpdateDelta 500

///
/// @class flxIoTArduino
///
/// @brief  A framework action that encapsulates the connection to the Arduino IoT Cloud
///
/// @note   Also implements the `flxIWriterJSON` interface, so it receives update data as a JSON doc.
///
class flxIoTArduino : public flxActionType<flxIoTArduino>, public flxIWriterJSON
{
  private:
    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Called when the network status changes.
    ///
    /// @param bConnected   Connection status - true connected, false not connected.
    ///
    void onConnectionChange(bool bConnected)
    {

        _canConnect = bConnected;

        if (!_isEnabled)
            return;

        if (_canConnect)
        {
            if (cloudAPISecret().empty() || cloudAPISecret().empty())
                flxLog_W(F("ArduinoIoT - Cloud API credentials are not provided"));

            if (deviceID().empty() || deviceSecret().empty())
                flxLog_W(F("ArduinoIoT - Device parameters not set"));

            if (thingName().empty())
                flxLog_W(F("ArduinoIoT - Thing Name not provided"));

            connect();
        }
        else
            disconnect();
    }

    void connect(void);
    void disconnect(void);

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Called to create a WiFi client object.
    /// @note   Will delete any current object and create a new one
    ///
    /// @return true on success, false on failure
    ///
    bool createWiFiClient(void)
    {
        if (_wifiClient)
            delete _wifiClient;

        _wifiClient = new WiFiClientSecure;
        if (!_wifiClient)
        {
            flxLog_E("Arduino IoT - Unable to allocate WiFi Client");
            return false;
        }

        _wifiClient->setInsecure(); // NOTE: This is from the examples provided by Arduino

        return true;
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for setting the enabled value for this action
    /// @param bEnabled - bool - true if the action should be set to enabled
    ///
    void set_isEnabled(bool bEnabled)
    {
        // Any changes?
        if (_isEnabled == bEnabled)
            return;

        _isEnabled = bEnabled;

        // Can we connect?
        if (_canConnect)
            onConnectionChange(_canConnect);
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for getting the enabled value for this action
    /// @return bool - true if enabled, false if not
    ///
    bool get_isEnabled(void)
    {
        return _isEnabled;
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for getting the thing name
    /// @return string - name of the thing
    ///
    std::string get_thingName(void)
    {
        return _thingName;
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for setting the thing name. Will also reset error flag
    ///
    void set_thingName(std::string name)
    {
        _thingName = name;

        // reset error state - can try again
        _hadError = false;
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for getting the thing name
    /// @return string - ID of the thing
    ///
    std::string get_thingID(void)
    {
        return _thingID;
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Property callback for setting the thing ID. Will also reset error flag
    ///
    void set_thingID(std::string theID)
    {
        _thingID = theID;

        // reset error state - can try again
        _hadError = false;
    }

  public:
    flxIoTArduino()
        : _isEnabled{false}, _canConnect{false}, _theNetwork{nullptr}, _wifiClient{nullptr}, _tokenTicks{0},
          _bInitialized{false}, _lastArduinoUpdate{0}, _startupCounter{0}, _loopTimeLimit{kArduinoIoTUpdateDelta},
          _thingValid{false}, _hadError{false}, _fallbackID{false}
    {
        setName("Arduino IoT", "Connection to Arduino IoT Cloud");

        // Register our properties
        flxRegister(enabled, "Enabled", "Enable or Disable the Arduino IoT Client");

        flxRegister(thingName, "Thing Name", "The Thing Name to use for the IoT Device connection");

        flxRegister(thingID, "Thing ID", "The Thing ID to use for the IoT Device connection");

        flxRegister(cloudAPIClientID, "API Client ID", "The Arduino Cloud API Client ID");

        flxRegister(cloudAPISecret, "API Secret", "The Arduino Cloud API Secret");

        flxRegister(deviceSecret, "Device Secret", "The Arduino IoT Device Secret");

        flxRegister(deviceID, "Device ID", "The Arduino IoT Device ID");

        flux.add(this);
    }

    //----------------------------------------------------------------------
    ~flxIoTArduino()
    {
        if (_wifiClient != nullptr)
            delete _wifiClient;

        for (auto x : _parameterToVar)
        {
            if (x.second)
            {
                if (x.second->variable)
                    delete x.second->variable;
                delete x.second;
            }
        }
    }

    //----------------------------------------------------------------------
    ///
    /// @brief  Inteface method - called with data to write to the cloud
    ///
    void write(JsonDocument &jsonDoc);

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Used to register for network events .
    ///
    /// @param theEvent     The Event object to register with
    ///
    void listenToConnection(flxSignalBool &theEvent)
    {
        // Register to get notified on connection changes
        theEvent.call(this, &flxIoTArduino::onConnectionChange);
    }

    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  API method used to set the system network connection.
    /// @note   Will register for network connectivity change events
    /// @param theNetwork   The network object.
    ///
    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Are we connected to the cloud?
    /// @return true if connected yes, false if not connected
    ///
    bool connected()
    {
        return (_isEnabled && _canConnect);
    }

    ///
    /// @brief  Loop for processing events
    ///
    bool loop(void);

    // Name of this thing in Arduino IOT - use this if we need to create a thing ...
    flxPropertyRWString<flxIoTArduino, &flxIoTArduino::get_thingName, &flxIoTArduino::set_thingName> thingName;

    // ArduinoIoT Thing ID.
    flxPropertyRWSecureString<flxIoTArduino, &flxIoTArduino::get_thingID, &flxIoTArduino::set_thingID> thingID;

    // Arduino Cloud API client id -
    flxPropertySecureString<flxIoTArduino> cloudAPIClientID;

    // Arduino Cloud API secret
    flxPropertySecureString<flxIoTArduino> cloudAPISecret;

    // ArduinoIOT Device secret
    flxPropertySecureString<flxIoTArduino> deviceSecret;

    // ArduinoIOT Device ID
    flxPropertySecureString<flxIoTArduino> deviceID;

    // Enabled/Disabled
    flxPropertyRWBool<flxIoTArduino, &flxIoTArduino::get_isEnabled, &flxIoTArduino::set_isEnabled> enabled;

  private:
    bool getArduinoToken(void);
    bool checkToken(void);
    int postJSONPayload(const char *url, JsonDocument &jIn);
    bool setupArduinoThing(void);
    bool createArduinoIoTVariable(char *szNameBuffer, uint32_t hash_id, flxDataType_t dataType);
    flxDataType_t getValueType(JsonPair &kvValue);
    void updateArduinoIoTVariable(flxIoTArduinoVar_t *value, JsonPair &kvParam);
    bool linkToCloudVariable(char *szNameBuffer, uint32_t hash_id, flxDataType_t dataType);
    void freeVariableMap(void);
    bool getThingIDFallback(void);
    ///---------------------------------------------------------------------------------------
    ///
    /// @brief  Template to create and register a local cloud variable with a cloud variable on Arduino IoT
    ///
    /// @param szName   Name of the IoT Cloud variable
    /// @param pValue   The struct that contains the local Cloud Variable
    /// @tparam T The type of the cloud variable
    /// @return true on success, false on failure
    ///
    template <typename T> bool registerArduinoVariable(char *szName, flxIoTArduinoVar_t *pValue)
    {
        // Create the local Cloud Variable - of type T
        pValue->variable = (void *)new T();
        if (!pValue->variable)
            return false;

        // Register with the IoT Cloud
        ArduinoCloud.addPropertyReal(*((T *)pValue->variable), szName, READ, ON_CHANGE);

        return true;
    }

    // This object is enabled?
    bool _isEnabled;

    // Can we connect? is the network here?
    bool _canConnect;

    flxNetwork *_theNetwork;

    WiFiClientSecure *_wifiClient;

    // For our arduino interactions
    std::string _arduinoToken;

    uint32_t _tokenTicks;

    // Our variable map - [hash of full parameter name, pointer to a flxIoTArduinoVar_t struct]
    std::map<uint32_t, flxIoTArduinoVar_t *> _parameterToVar;

    bool _bInitialized;

    // Keep track update() calls to arduinoIoT - use this at startup to pump the init process
    // of the system...
    //
    // Delta between updates - last update ticks
    uint32_t _lastArduinoUpdate;

    // Count of our start up updates calls to Arduino IoT
    int _startupCounter;

    // Create an instance of our version of the connection handler
    DataLoggerAIOTConnectionHandler _myConnectionHandler;

    // loop time limit
    uint32_t _loopTimeLimit;

    bool _thingValid;
    bool _hadError; // error in config/setup -- needs updating
    bool _fallbackID;

    std::string _thingID;
    std::string _thingName;
};
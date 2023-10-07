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
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <ArduinoIoTCloud.h>

#include <map>

// The variable / output parameter name is mapped to a local ArduinoIot Cloud variable
// These variables have a limited type set, and are actually objects to dynamically detect
// when values are changed. This enables value updates to the cloud from the device/thing.
//
// To map this the following is done:
//      - Flux Output parameter names are <device name>-<parameter name>
//      - The names are truncated to 64, left justified if needed when creating cloud vars
//      - The name used in our map object is the hash of the full name
//      - When a variable is created in the cloud, the local var is allocated and added to the map
//      - The map value is a struct that contains a type code (flxDataType_t) and a void * pointer

typedef struct
{
    flxDataType_t type;
    void *variable;
} flxIoTArduinoVar_t;

class flxIoTArduino : public flxActionType<flxIoTArduino>, public flxIWriterJSON
{
  private:
    // Event callback
    //----------------------------------------------------------------------------
    void onConnectionChange(bool bConnected)
    {
        _canConnect = bConnected;

        /////////////////////////////////////
        // TOOD - Check parameters here ...
        /////////////////////////////////////
    }

    bool createWiFiClient(void)
    {
        if (_wifiClient)
            delete _wifiClient;

        _wifiClient = new WiFiClientSecure;
        if (!_wifiClient)
        {
            flxLog_E("Arduion IoT - Unable to allocate WiFi Client");
            return false;
        }

        _wifiClient->setInsecure(); // NOTE: This is from the examples provided by Arduino

        return true;
    }

  public:
    flxIoTArduino() : _isEnabled{false}, _canConnect{false}, _theNetwork{nullptr}, _wifiClient{nullptr}
    {
        setName("Arduino IoT", "Connection to Arduino IoT Cloud");

        // Register our properties

        flxRegister(thingName, "Thing Name", "The Thing Name to use for the IoT Device connection");

        flxRegister(cloudAPIClientID, "API Client ID", "The Arduino Cloud API Client ID");

        flxRegister(cloudAPISecret, "API Secret", "The Arduino Cloud API Secret");

        flxRegister(deviceSecret, "Device Secret", "The ArduinoIoT Device Secret");

        flxRegister(deviceID, "Device ID", "The ArduinoIoT Device ID");

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
    void write(JsonDocument &jsonDoc);

    // TODO VISIT
    bool initialize(void)
    {
    }


    //----------------------------------------------------------------------    
    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenToConnection(flxSignalBool &theEvent)
    {
        // Register to get notified on connection changes
        theEvent.call(this, &flxIoTArduino::onConnectionChange);
    }

    void setNetwork(flxNetwork *theNetwork)
    {
        _theNetwork = theNetwork;

        listenToConnection(theNetwork->on_connectionChange);
    }
    bool connected()
    {
        return (_isEnabled && _canConnect);
    }

    // Name of this thing in Arduino IOT
    flxPropertyString<flxIoTArduino> thingName;

    // Arduino Cloud API client id -
    flxPropertySecureString<flxIoTArduino> cloudAPIClientID;

    // Arduino Cloud API secret
    flxPropertySecureString<flxIoTArduino> cloudAPISecret;

    // ArduinoIOT Device secret
    flxPropertySecureString<flxIoTArduino> deviceSecret;

    // ArduinoIOT Device ID
    flxPropertySecureString<flxIoTArduino> deviceID;

  private:

    bool getArduinoToken(void);
    bool postJSONPayload(const char *url, JsonDocument &jIn, JsonDocument &jOut);
    bool createArduinoThing(void);
    bool createArduinoIoTVariable(char *szNameBuffer, uint32_t hash_id, flxDataType_t dataType);
    flxDataType_t getValueType(JsonPair &kvValue);
    void updateArduinoIoTVariable(flxIoTArduinoVar_t *value, JsonPair &kvParam);

    // template to register variables with the cloud
    template <typename T> bool registerArduinoVariable(char * szName, flxIoTArduinoVar_t * pValue)
    {

        pValue->variable = (void*)new T();
        if (!pValue->variable)
            return false;
        ArduinoCloud.addPropertyReal( *((T*)pValue->variable), szName, READ, ON_CHANGE);
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
    std::string _arduinoThingID;

    // Our variable map - [hash of full parameter name, pointer to a flxIoTArduinoVar_t struct]
    std::map<uint32_t, flxIoTArduinoVar_t *> _parameterToVar;
};
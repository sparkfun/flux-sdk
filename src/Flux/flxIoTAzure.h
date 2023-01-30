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

#include "flxMQTTESP32.h"

#include <az_core.h>
#include <az_iot.h>
#include <cstdlib>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////////////////
// NOTE:
//   This object requires the installation of the Azure SDK for C Arduino library
//
////////////////////////////////////////////////////////////////////////////////////////////

// From the Azure SDK examples. Changing this randomly causes connectoin failures
#define AZURE_SDK_CLIENT_USER_AGENT "c%2F" AZ_SDK_VERSION_STRING "(ard;esp32)"

// This is copied from examples in the Azure C SDK. An default constructor and initialize() method were
// added to allow post creation setup of the object. 

class AzIoTSasToken
{
  public:
    AzIoTSasToken() : sasToken{AZ_SPAN_EMPTY}
    {
    }

    AzIoTSasToken(az_iot_hub_client *client, az_span deviceKey, az_span signatureBuffer, az_span sasTokenBuffer);
    void initialize(az_iot_hub_client *client, az_span deviceKey, az_span signatureBuffer, az_span sasTokenBuffer);
    int Generate(unsigned int expiryTimeInMinutes);
    bool IsExpired();
    az_span Get();

  private:
    az_iot_hub_client *client;
    az_span deviceKey;
    az_span signatureBuffer;
    az_span sasTokenBuffer;
    az_span sasToken;
    uint32_t expirationUnixTime;
};



//---------------------------------------------------------------------    
// simple class to support Azure IoT

class flxIoTAzure : public flxMQTTESP32SecureCore<flxIoTAzure>, public flxWriter
{
  public:
    flxIoTAzure() : _initialized{false}, _hubInitialized{false}, _connected{false}
    {
        setName("Azure IoT", "Connection to Azure IoT");

        flxRegister(deviceID, "Device ID", "The device id for the Azure IoT device");
        flxRegister(deviceKey, "Device Key", "The device key for the Azure IoT device");

        flux.add(this);
    }

    // for the Writer interface
    void write(int data)
    {
        // noop
    }
    void write(float data)
    {
        // noop
    }
    //---------------------------------------------------------------------    
    virtual void write(const char *value, bool newline)
    {
        if (!value || !_connected)
            return;

        flxMQTTESP32SecureCore::write(value, false);
    }

    //---------------------------------------------------------------------
    // Method mostly copyied this method from examples in the Azure SDK for C.
    bool initializeIoTHubClient()
    {

        if (_hubInitialized)
            return true;

        az_iot_hub_client_options options = az_iot_hub_client_options_default();
        options.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

        // need to move hostname to a persistant char array from std::string. The called function
        // assumes the value is persistant
        strlcpy(_az_host, server().c_str(), sizeof(_az_host));
        strlcpy(_az_device_id, deviceID().c_str(), sizeof(_az_device_id));

        if (az_result_failed(az_iot_hub_client_init(&_client, az_span_create((uint8_t *)_az_host, strlen(_az_host)),
                                                    az_span_create((uint8_t *)_az_device_id, strlen(_az_device_id)),
                                                    &options)))
        {
            flxLog_E(F("%s: Failed initializing Azure IoT Hub client"), name());
            return false;
        }

        size_t client_id_length;
        char mqtt_client_id[kBufferSize];
        if (az_result_failed(az_iot_hub_client_get_client_id(&_client, mqtt_client_id, sizeof(mqtt_client_id) - 1,
                                                             &client_id_length)))
        {
            flxLog_E(F("%s: Failed getting client id"), name());
            return false;
        }

        char mqtt_username[kBufferSize];
        if (az_result_failed(az_iot_hub_client_get_user_name(&_client, mqtt_username, 
                        sizeof(mqtt_username)/sizeof(mqtt_username[0]), NULL)))
        {
            flxLog_E(F("%s: Failed to get azure username."),name());
            return false;
        }

        // Update our properties with connected info
        username = mqtt_username;
        clientName = mqtt_client_id;

        _hubInitialized = true;
        return _hubInitialized;
    }

    //---------------------------------------------------------------------
    // override the connect method so that connection information can be accessed
    // from the Azure SDK

    bool connect()
    {
        // Do we need intializing
        if (!_initialized)
        {
            if (!initialize())
            {
                flxLog_E(F("%s failed to initialize"), name());
                return false;
            }
        }

        // Connect to the Azure IoT Hub
        if (!initializeIoTHubClient())
            return false;

        if (_sasToken.Generate(60) != 0)
        {
            flxLog_E(F("%s : Failed generating SAS authentication token"), name());
            return false;
        }

        // Get the token/Password
        char *token = (char *)az_span_ptr(_sasToken.Get());
        if (!token)
        {
            flxLog_E(F("%s: Error creating password token"), name());
            return false;
        }
        // set the password property to the token
        password = token;

        // If we are here, we can get our mqtt update topic

        char telemetry_topic[kBufferSize];

        if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(&_client, NULL, telemetry_topic,
                                                                           sizeof(telemetry_topic), NULL)))
        {
            flxLog_E(F("%s: Failed to retrieve azure mqtt topic from Azure SDK"), name());
            return false;
        }
        // Set the topic property
        topic = telemetry_topic;

        _connected = flxMQTTESP32SecureCore<flxIoTAzure>::connect();

        return _connected;
    }

    //---------------------------------------------------------------------
    // disconnect()

    void disconnect()
    {
        _connected = false;

        // call super
        flxMQTTESP32SecureCore<flxIoTAzure>::disconnect();
    }
    //---------------------------------------------------------------------
    // Initialize method for the class

    bool initialize(void)
    {
        if (_initialized || !enabled)
            return true;

        // do we have a device key
        if (deviceKey().length() < 10)
        {
            flxLog_E(F("%s: No device authentication key provided"), name());
            return false;
        }

        if (!flxMQTTESP32SecureCore::initialize())
            return false;

        // We need the key in persistant storage
        strlcpy(_az_device_key, deviceKey().c_str(), sizeof(_az_device_key));

        _sasToken.initialize(&_client, 
                az_span_create((uint8_t*)_az_device_key, strlen(_az_device_key)),
                az_span_create((uint8_t*)_sas_signature_buffer, sizeof(_sas_signature_buffer)),
                az_span_create((uint8_t*)_mqtt_password, sizeof(_mqtt_password)));                

        _initialized = true;
        return true;
    }

    //---------------------------------------------------------------------    
    // loop
    bool loop(void)
    {
        if (!_connected)
            return false;

        // did our sas token expire?
        if (_sasToken.IsExpired())
        {
            // the token needs to be refreshed, which updates the mqtt password. So disconnect
            // and reconnect
            disconnect();
            if (!connect())
            {
                flxLog_E(F("%s: Failed to reconnect after auth token refresh."), name());
                return false;
            }else
                flxLog_I(F("%s: SAS Auth token refreshed"), name());
            return true;
        }
        return false;
    }

    // Properties
    flxPropertyString<flxIoTAzure> deviceID;
    flxPropertySecureString<flxIoTAzure> deviceKey;

private:
    static constexpr uint kBufferSize = 128;
    static constexpr uint kBufferSize2X = kBufferSize * 2;
    // The azure C SDK expects many of the passed in values to be persistant values, but
    // our props are std::strings, which are dynamic. So create buffers in this object
    // to manage the connection data for the SDK.

    char _mqtt_password[kBufferSize2X];
    uint8_t _sas_signature_buffer[kBufferSize2X];

    az_iot_hub_client _client;

    char _az_device_id[kBufferSize];    // we need persistant storage for the device id
    char _az_host[kBufferSize];         // we need persistant, char array, storage for this
    char _az_device_key[kBufferSize];    // "" "" "" 

    bool _initialized;
    bool _hubInitialized;
    bool _connected;

    AzIoTSasToken _sasToken;
};
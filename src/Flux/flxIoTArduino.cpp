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

#include "flxIoTArduino.h"
#include "flxUtils.h"



// Arduino IOT Constants

const char * kArduinoIoTAPIServer = "api2.arduino.cc";
#define kArduinoIoTAPIPort 443

const char * kArduinoIoTThingsPath = "/iot/v2/things";

//---------------------------------------------------------------------------------------
//
// Call to Arduino IoT to get out session token

bool flxIoTArduino::getArduinoToken(void)
{

    if (cloudAPISecret().length() == 0 || cloudAPIClientID().length() == 0)
    {
        flxLog_E(F("Arduino Cloud API credintials not provided. ArduinoIoT not available"));
        return false;
    }

    if (!_wifiClient && !createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to Wi-Fi"));
        return false;
    }

    HTTPClient http;

    char szURL[256];
    snprintf(szURL, sizeof(szURL), "%s:%d/%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, "/iot/v1/clients/token");

    if (!http.begin(*_wifiClient, szURL))
    {
        flxLog_E(F("%s: Error reaching URL: %s"), this->name(), szURL);
        return false;
    }

    // Prepare the HTTP request
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    std::string postData = "grant_type=client_credentials&client_id" + cloudAPIClientID();
    postData += "&client_secret=" + cloudAPISecret();
    postData += "&audience=https://api2.arduino.cc/iot";

    int rc = http.POST((uint8_t*)postData.c_str(), postData.length());

    if (rc != 201 || rc != 200)
    {
        flxLog_E(F("ArduinoIoT communication error - token request"));
        return false;
    }

    StaticJsonDocument<200> jDoc;
    if (deserializeJson(jDoc, http.getString()) != DeserializationError::Ok)
    {
        flxLog_E(F("Unable to parse Arduino IoT token return value."));
        return false;
    }

    if (jDoc.containsKey("access_token"))
        _arduinoToken = jDoc["access_token"].as<const char *>();
    else
    {
        flxLog_E(F(" Arduino IoT token not returned."));
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------------------
// General PUT call for json payloads
//
bool flxIoTArduino::postJSONPayload(const char* url, JsonDocument &jIn, JsonDocument &jOut)
{

    if (!_wifiClient && !createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to Wi-Fi")); 
        return false;
    }

    HTTPClient http;

    char szURL[256];
    snprintf(szURL, sizeof(szURL), "%s:%d/%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, url);

    if (!http.begin(*_wifiClient, szURL))
    {
        flxLog_E(F("%s: Error reaching URL: %s"), this->name(), szURL);
        return false;
    }

    // Prepare the HTTP request
    std::string bearer = "Bearer " + _arduinoToken;
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", bearer.c_str());

    char szBuffer[256];

    size_t nWritten = serializeJson(jIn, szBuffer, sizeof(szBuffer));

    if (!nWritten)
    {
        flxLog_E(F("ArduinoIoT - error parsing request body"));
        return false;
    }

    int rc = http.PUT((uint8_t *)szBuffer, nWritten);

    // valid response - we are looking for a 201
    if (rc != 201)
    {
        flxLog_E(F("ArduinoIoT Cloud - communication failure. Error: %d"), rc);
        return false;
    }

    if (deserializeJson(jOut, http.getString()) != DeserializationError::Ok)
    {
        flxLog_E(F("ArduinoIoT communication - invalid response."));
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------
bool flxIoTArduino::createArduinoThing(void)
{

    // do we have a token?
    if (_arduinoToken.length() == 0)
    {
        if (!getArduinoToken())
        {
            flxLog_E(F("Arduino IoT Cloud not available, or account credintials incorrect"));
            return false;
        }
    }

    if (thingName().length() == 0 || deviceID().length() == 0)
    {
        flxLog_E(F("Device/Thing Name not provided. Unable to continue with ArduinoIoT"));
        return false;
    }

    // Create our payload

    StaticJsonDocument<200> jDoc;
    jDoc["device_id"] = deviceID();
    jDoc["name"] = thingName();

    // Create our output
    StaticJsonDocument<200> jOut;

    if (!postJSONPayload(kArduinoIoTThingsPath, jDoc, jOut))
    {
        flxLog_E(F("ArduinoIoT Cloud not available. Unable to create thing"));
        return false;
    }

    if (jOut.containsKey("id"))
        _arduinoThingID = jDoc["id"].as<const char *>();
    else
    {
        flxLog_E(F(" Arduino IoT Thing Token not returned."));
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------------------
// Create a variable in the IOT Cloud - in our "Thing" and then associate it with a local variable
// 
bool flxIoTArduino::createArduinoIoTVariable(char *szNameBuffer, uint32_t hash_id, flxDataType_t dataType)
{

    // do we have a thing ID?
    if (_arduinoThingID.length() == 0)
    {
        if (!createArduinoThing())
        {
            flxLog_E(F("Arduino IoT Cloud not available"));
            return false;
        }
    }

    // Build our payload
    StaticJsonDocument<256> jDoc;

    jDoc["name"] = szNameBuffer;          // The friendly name of the property
    jDoc["variable_name"] = szNameBuffer; // The sketch variable name of the property
    jDoc["permission"] = "READ_WRITE";
    jDoc["update_strategy"] = "ON_CHANGE"; // "TIMED"

    // note - low level types seem limited in the IoT cloud space
    switch (dataType)
    {
    case flxTypeUInt:
    case flxTypeInt:
    case flxTypeBool:
        jDoc["type"] = "INT";
        break;

    case flxTypeFloat:
        jDoc["type"] = "FLOAT";
        break;

    case flxTypeString:
        jDoc["type"] = "CHARSTRING";
        break;

    default:
        // should never get here really
        flxLog_E(F("Unknown data type for ArduinoIoT: %d"), (int)dataType);
        return false;
    }

    // output /result document
    StaticJsonDocument<200> jOut;

    // get the correct path
    char szPath[128];
    snprintf(szPath, sizeof(szPath), "%s/%s/properties", kArduinoIoTThingsPath, _arduinoThingID.c_str());

    if (!postJSONPayload(szPath, jDoc, jOut))
    {
        flxLog_E(F("ArduinoIoT Cloud not available. Unable to create variable %s"), szNameBuffer);
        return false;
    }

    if (!jOut.containsKey("id"))
    {
        flxLog_E(F(" Arduino IoT create variable failed - no ID returned from server."));
        return false;
    }

    /////////////////////////////
    // TODO -- refactor out

    // Okay, at this point we have a variable in the server for our thing, now we need to
    // map this to a local value

    flxIoTArduinoVar_t *pValue = new flxIoTArduinoVar_t;

    if (!pValue)
    {
        flxLog_E(F("ArduinoIoT - failure to create local variable. %s"), szNameBuffer);
        return false;
    }

    pValue->type = dataType;

    bool status;
    // note - low level types seem limited in the IoT cloud space
    switch (dataType)
    {
    case flxTypeUInt:
        status = registerArduinoVariable<CloudUnsignedInt>(szNameBuffer, pValue);
        break;

    case flxTypeInt:
        status = registerArduinoVariable<CloudInt>(szNameBuffer, pValue);
        break;

    case flxTypeBool:
        status = registerArduinoVariable<CloudBool>(szNameBuffer, pValue);
        break;

    case flxTypeFloat:
        status = registerArduinoVariable<CloudFloat>(szNameBuffer, pValue);
        break;

    case flxTypeString:
        status = registerArduinoVariable<CloudString>(szNameBuffer, pValue);
        break;
    }
    if (!status)
    {
        delete pValue;
        flxLog_E(F("ArduinoIoT - failure to create local Ardiuno variable. %s"), szNameBuffer);
        return false;
    }

    // Now add this to the map

    _parameterToVar[hash_id] = pValue;

    return true;
}

//---------------------------------------------------------------------------------------
// Get the basic type of the value in the json pair - return type code
flxDataType_t flxIoTArduino::getValueType(JsonPair &kv)
{

    if (kv.value().is<float>())
        return flxTypeFloat;
    if (kv.value().is<double>())
        return flxTypeFloat;

    else if (kv.value().is<const char *>())
        return flxTypeString;

    else if (kv.value().is<signed int>())
        return flxTypeInt;
    else if (kv.value().is<signed short>())
        return flxTypeInt;
    else if (kv.value().is<signed long>())
        return flxTypeInt;

    else if (kv.value().is<unsigned int>())
        return flxTypeUInt;
    else if (kv.value().is<unsigned short>())
        return flxTypeUInt;
    else if (kv.value().is<unsigned long>())
        return flxTypeUInt;

    else if (kv.value().is<bool>())
        return flxTypeBool;

    return flxTypeNone;
}

//---------------------------------------------------------------------------------------
void flxIoTArduino::updateArduinoIoTVariable(flxIoTArduinoVar_t *value, JsonPair &kv)
{

    switch (value->type)
    {
    case flxTypeBool:
        *((CloudBool *)value->variable) = kv.value().as<bool>();
        break;

    case flxTypeInt:
        *((CloudInt *)value->variable) = kv.value().as<signed int>();
        break;

    case flxTypeUInt:
        *((CloudUnsignedInt *)value->variable) = kv.value().as<unsigned int>();
        break;

    case flxTypeFloat:
        *((CloudFloat *)value->variable) = kv.value().as<float>();
        break;

    case flxTypeString:
        *((CloudString *)value->variable) = kv.value().as<const char *>();
        break;
    }
}

void flxIoTArduino::write(JsonDocument &jDoc)
{

    // The Plan:
    // Loop over the objects in the json document, each object is a device. For each
    // device, loop over the data values/output parameters and
    //	- Create a name - 64 char max
    //	- hash name, see if it's in the parameter map objecvt
    //		- Yes, update value
    //		- No - create and register variable in Arduino cloud, then update value

    if (!_isEnabled || !_canConnect)
        return;

    JsonObject jObj;

    char szNameBuffer[65];
    uint32_t hash_id;
    flxDataType_t dataType;

    JsonObject jRoot = jDoc.as<JsonObject>();
    for (JsonPair kvObj : jRoot)
    {

        // Go through the parameters in the object

        for (JsonPair kvParam : kvObj.value().as<JsonObject>())
        {

            // we need the data type.
            dataType = getValueType(kvParam);

            // make a name
            snprintf(szNameBuffer, sizeof(szNameBuffer), "%s_%s", kvObj.key().c_str(), kvParam.key().c_str());

            hash_id = flx_utils::id_hash_string(szNameBuffer);

            // Is this value in our parameter map?
            auto itSearch = _parameterToVar.find(hash_id);

            // No match?
            if (itSearch == _parameterToVar.end())
            {
                // Need to create a ArduinoIoT variable for this parameter
                if (!createArduinoIoTVariable(szNameBuffer, hash_id, dataType))
                {
                    flxLog_E(F("Error creating ArduinoIoT Cloud variable for parameter %s"), szNameBuffer);
                    continue;
                }
                // redo our search
                itSearch = _parameterToVar.find(hash_id);
                if (itSearch == _parameterToVar.end())
                {
                    // Something is really wrong and we need to abort
                    flxLog_E(
                        F("Error finding ArduinoIoT Cloud variable in parameter list. Skipping Arduino IoT data log"));
                    return;
                }
            }

            // Okay - we have a value, now we need to update the parameter

            updateArduinoIoTVariable(itSearch->second, kvParam);
        }
    }
}
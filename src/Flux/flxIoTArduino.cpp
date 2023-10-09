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

/// @brief Arduino IoT Server
#define kArduinoIoTAPIServer "https://api2.arduino.cc"

/// @brief Arduino IoT Server Port
#define kArduinoIoTAPIPort 443

/// @brief URL path for IoT Things endpoint
#define kArduinoIoTThingsPath "/iot/v2/things"

/// @brief URL path for IoT Things token end point
#define kArduinoIoTTokenPath "/iot/v1/clients/token"


///---------------------------------------------------------------------------------------
///
/// @brief     Creates a valid arduino variable name
//  @param[In,Out] szVariable the name to convert 
///
/// @return true on success, false on failure
///
bool flxIoTArduino::validateVariableName(char *szVariable)
{
    if (!szVariable)
        return false;

    size_t nChar = strlen(szVariable);

    int idst = 0;
    int isrc = 0;
    for (; isrc < nChar; isrc++)
    {
        if (std::isalnum(szVariable[isrc]) || szVariable[isrc] == '_' )
            szVariable[idst++] = szVariable[isrc];
    }
    szVariable[idst] = '\0';

    return (strlen(szVariable) > 1);
}
///---------------------------------------------------------------------------------------
///
/// @brief      Call to Arduino IoT Cloud to get out session token.
/// @note    This method handles all HTTP interactions and sets the token instance
/// token instance data. The Cloud API credentials are required for this method
/// to execute successfully.
///
/// @return true on success, false on failure
///
bool flxIoTArduino::getArduinoToken(void)
{

    // No values, no dice
    if (cloudAPISecret().length() == 0 || cloudAPIClientID().length() == 0)
    {
        flxLog_E(F("Arduino Cloud API credentials not provided. ArduinoIoT not available"));
        return false;
    }

    // Network connection?
    if (!_wifiClient && !createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to Wi-Fi"));
        return false;
    }

    HTTPClient http;
    char szURL[256];

    // Our full URL
    snprintf(szURL, sizeof(szURL), "%s:%d%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, kArduinoIoTTokenPath);

    if (!http.begin(*_wifiClient, szURL))
    {
        flxLog_E(F("%s: Error reaching URL: %s"), this->name(), szURL);
        return false;
    }

    // Prepare the HTTP request
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // payload
    std::string postData = "grant_type=client_credentials&client_id" + cloudAPIClientID();
    postData += "&client_secret=" + cloudAPISecret();
    postData += "&audience=https://api2.arduino.cc/iot";

    int rc = http.POST((uint8_t *)postData.c_str(), postData.length());

    if (rc != 201 || rc != 200)
    {
        flxLog_E(F("ArduinoIoT HTTP communication error [%d] - token request"), rc);
        return false;
    }

    // results
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
///
/// @brief A general PUT call for json payloads - used in this class
///
/// @param url  The full URL of the endpoint to PUT to
/// @param jIn  JsonDocument that contains the payload for the PUT call
/// @param jOut JsonDocument that contains the returned json payload
/// @return true on success, false on failure
///
bool flxIoTArduino::postJSONPayload(const char *url, JsonDocument &jIn, JsonDocument &jOut)
{

    if (!url || strlen(url) < 1)
    {
        flxLog_E(F("Arduino IoT - Invalid URL provided"));
        return false;
    }
    if (!_wifiClient && !createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to Wi-Fi"));
        return false;
    }

    HTTPClient http;

    char szURL[256];
    char slash = '\0';
    if (url[0] != '/')
        slash = '/';

    snprintf(szURL, sizeof(szURL), "%s:%d%c%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, slash, url);

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
        flxLog_E(F("ArduinoIoT Cloud - http communication failure. Error: %d"), rc);
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
///
/// @brief  Create a *thing* in the Arduino IoT cloud of a given thing
///
/// @note   Requires a Arduino IoT token from the cloud. Uses the 'Thing Name' and
///         'Device ID' properties of this action
///
/// @return true on success, false on failure
///
bool flxIoTArduino::createArduinoThing(void)
{

    // do we have a token?
    if (_arduinoToken.length() == 0)
    {
        if (!getArduinoToken())
        {
            flxLog_E(F("Arduino IoT Cloud not available or account credentials incorrect"));
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

///---------------------------------------------------------------------------------------
///
/// @brief  Connect a local Arduino IoT variable to the IoT Cloud
///
/// @param szNameBuffer Name of the IoT Cloud variable
/// @param hash_id      Hash ID of the name - used locally to map the name to the variable
/// @param dataType     Type of the variable - uses flux data types
/// @return true on success, false on failure
///
bool flxIoTArduino::linkToCloudVariable(char *szNameBuffer, uint32_t hash_id, flxDataType_t dataType)
{

    flxIoTArduinoVar_t *pValue = new flxIoTArduinoVar_t;

    if (!pValue)
    {
        flxLog_E(F("ArduinoIoT - failure to create local variable. %s"), szNameBuffer);
        return false;
    }

    pValue->type = dataType;

    bool status = false;

    if (dataType == flxTypeUInt)
        status = registerArduinoVariable<CloudUnsignedInt>(szNameBuffer, pValue);
    else if (dataType == flxTypeInt)
        status = registerArduinoVariable<CloudInt>(szNameBuffer, pValue);
    else if (dataType == flxTypeBool)
        status = registerArduinoVariable<CloudBool>(szNameBuffer, pValue);
    else if (dataType == flxTypeFloat)
        status = registerArduinoVariable<CloudFloat>(szNameBuffer, pValue);
    else if (dataType == flxTypeString)
        status = registerArduinoVariable<CloudString>(szNameBuffer, pValue);

    if (!status)
    {
        delete pValue;
        flxLog_E(F("ArduinoIoT - failure to create local Arduino variable. %s"), szNameBuffer);
        return false;
    }

    // Now add this variable to the map - indexed by our hash id
    _parameterToVar[hash_id] = pValue;

    return true;
}
///---------------------------------------------------------------------------------------
///
/// @brief  Create a variable in the IOT Cloud - in our "Thing" and then associate it with a local variable
///
/// @note   This method requires a Thing be available/created on the Arduino cloud.
/// @param szNameBuffer Name of the IoT Cloud variable
/// @param hash_id      Hash ID of the name - used locally to map the name to the variable
/// @param dataType     Type of the variable - uses flux data types
/// @return true on success, false on failure
///
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

    // Okay, at this point we have a variable in the server for our thing, now we need to
    // map this to a local value

    return linkToCloudVariable(szNameBuffer, hash_id, dataType);
}

//---------------------------------------------------------------------------------------
///
/// @brief      Get the basic type of the value in the json pair - return type code
///
/// @param kv   JsonPair  - The value of this pair is used to determine value type
/// @return     true on success, false on failure
///
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
///
/// @brief          Updates the value of the local Arduino IoT variable
/// @note           When a value is set to the local variable, the new value is updated on
///                 the Arduino IoT Cloud automatically.
/// @param value    The value struct that contains the type and Cloud Variable object
/// @param kv       JsonPair  - The value of this pair is used to determine value type
///
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
//---------------------------------------------------------------------------------------
///
/// @brief          Receives a Json Document that contains the current values to output
/// @note           This method is part of the `flxIWriterJSON` interface
/// @param jDoc     The JSON Document that contains the values to output.
///
void flxIoTArduino::write(JsonDocument &jDoc)
{

    // The Plan:
    // Loop over the objects in the json document, each object is a device. For each
    // device, loop over the data values/output parameters and:
    //	- Create a name - 64 char max
    //	- hash name, see if it's in the parameter map object
    //		- Yes, update value
    //		- No - create and register variable in Arduino cloud, then update value

    if (!_isEnabled || !_canConnect)
        return;

    JsonObject jObj;
    char szNameBuffer[65];
    uint32_t hash_id;
    flxDataType_t dataType;

    JsonObject jRoot = jDoc.as<JsonObject>(); // needed so we can iterate over object
    for (JsonPair kvObj : jRoot)
    {
        // Go through the parameters in the object

        for (JsonPair kvParam : kvObj.value().as<JsonObject>())
        {
            // we need the data type.
            dataType = getValueType(kvParam);

            // make a name
            snprintf(szNameBuffer, sizeof(szNameBuffer), "%s_%s", kvObj.key().c_str(), kvParam.key().c_str());
            if (!validateVariableName(szNameBuffer))
            {
                flxLog_E(F("ArduinoIoT - unable to create valid variable name: %s"), kvParam.key().c_str());
                continue;
            }
            
            hash_id = flx_utils::id_hash_string(szNameBuffer); // hash name

            flxLog_I("Arduino Variable - name: %s, type: %d, hash: %u", szNameBuffer, (int)dataType, hash_id);
            // Is this value in our parameter map?
            auto itSearch = _parameterToVar.find(hash_id);

            // No match?
            if (itSearch == _parameterToVar.end())
            {
                flxLog_I("Creating Variable: %s", szNameBuffer);

                // Need to create a ArduinoIoT variable for this parameter
                if (!createArduinoIoTVariable(szNameBuffer, hash_id, dataType))
                {
                    flxLog_E(F("Error creating ArduinoIoT Cloud variable for parameter: %s"), szNameBuffer);
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
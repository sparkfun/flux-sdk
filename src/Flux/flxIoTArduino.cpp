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
#include "flxSettings.h"
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

// The First ~10 calls to Arduino Cloud update sets up the system. So we
// make these calls quickly at start up in the loop method.
//
// The following helps manage this.
// Define the delta between arduino IoT cloud update calls during setup.
#define kArduinoIoTUpdateDelta 500
#define kArduinoIoTStartupLimit 40

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
        if (std::isalnum(szVariable[isrc]) || szVariable[isrc] == '_')
            szVariable[idst++] = szVariable[isrc];
    }
    szVariable[idst] = '\0';

    return (strlen(szVariable) > 1);
}

void flxIoTArduino::connect(void)
{

    if (deviceID().length() == 0 || deviceSecret().length() == 0)
    {
        flxLog_E(F("%d: Device credentials (ID, Secret) not set - unable to continue"), this->name());
        return;
    }

    if (!_bInitialized)
    {
        ArduinoCloud.setBoardId(deviceID().c_str());
        ArduinoCloud.setSecretDeviceKey(deviceSecret().c_str());

        // begin our session with the ArduinoCloud - pass in our special Connection Handler
        if (!ArduinoCloud.begin(_myConnectionHandler))
        {
            flxLog_E(F("%s: Error initializing the Arduino IoT Cloud subsystem"), this->name());
            return;
        }
        // Set the ArduinoCloud debug level - it's a function - global - annoying
        setDebugMessageLevel(DBG_ERROR);
        _bInitialized = true;
        _lastArduinoUpdate = millis();
        _startupCounter = 0;
        _myConnectionHandler.setConnected(true);
    }
}

void flxIoTArduino::disconnect(void)
{
    _myConnectionHandler.setConnected(false);
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

    // Network connection? Need a new one each interaction ...
    if (!createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to WiFi"));
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

    // // payload
    char szPayload[212];
    snprintf(szPayload, sizeof(szPayload),
             "grant_type=client_credentials&client_id=%s&client_secret=%s&audience=https://api2.arduino.cc/iot",
             cloudAPIClientID().c_str(), cloudAPISecret().c_str());

    int rc = http.POST((uint8_t *)szPayload, strlen(szPayload));

    if (rc < 200 || rc > 220)
    {
        flxLog_E(F("ArduinoIoT HTTP communication error [%d] - token request"), rc);
        http.end();
        return false;
    }

    // The return value of the token can be large, so use a dynamic json doc - stack based ...
    int retSize = http.getSize();

    // results
    DynamicJsonDocument jDoc(retSize + 100);

    if (deserializeJson(jDoc, http.getString()) != DeserializationError::Ok)
    {
        flxLog_E(F("Unable to parse Arduino IoT token return value."));
        http.end();
        return false;
    }
    http.end();

    if (jDoc.containsKey("access_token"))
    {
        _arduinoToken = jDoc["access_token"].as<const char *>();

        // oauth timeout
        _tokenTicks = millis() + 1000 * (jDoc["expires_in"].as<int>() - 1);
    }
    else
    {
        flxLog_E(F(" Arduino IoT token not returned."));
        return false;
    }

    return true;
}

bool flxIoTArduino::checkToken(void)
{
    // do we have a token?
    if (_arduinoToken.length() == 0 || millis() > _tokenTicks)
    {
        if (!getArduinoToken())
        {
            flxLog_E(F("Arduino IoT Cloud not available or account credentials incorrect"));
            return false;
        }
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
/// @return -1 on error, http return code otherwise
///
int flxIoTArduino::postJSONPayload(const char *url, JsonDocument &jIn, JsonDocument &jOut)
{

    if (!url || strlen(url) < 1)
    {
        flxLog_E(F("Arduino IoT - Invalid URL provided"));
        return -1;
    }
    if (!createWiFiClient())
    {
        flxLog_E(F("Arduino IoT Unable to connect to WiFi"));
        return -1;
    }

    if (_arduinoToken.length() == 0)
    {
        flxLog_E(F("No Arduino Cloud authentication set"));
        return -1;
    }
    HTTPClient http;

    char szURL[256];
    snprintf(szURL, sizeof(szURL), "%s:%d/%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, url + (url[0] == '/' ? 1 : 0));

    if (!http.begin(*_wifiClient, szURL))
    {
        flxLog_E(F("%s: Error reaching URL: %s"), this->name(), szURL);
        return -1;
    }

    // Prepare the HTTP request
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(_arduinoToken.c_str());
    http.setAuthorizationType("Bearer");

    char szBuffer[256];

    size_t nWritten = serializeJson(jIn, szBuffer, sizeof(szBuffer));

    if (!nWritten)
    {
        flxLog_E(F("ArduinoIoT - error parsing request body"));
        http.end();
        return -1;
    }

    int rc = http.PUT((uint8_t *)szBuffer, nWritten);
    // flxLog_E("RC IS: %d", rc);

    // valid response - we are looking for a 201
    if (rc != 201)
    {
        http.end();
        return rc;
    }

    // flxLog_I("SIZE OF OUTPUT: %d", http.getSize());
    if (deserializeJson(jOut, http.getString()) != DeserializationError::Ok)
    {
        flxLog_E(F("ArduinoIoT communication - invalid response."));
        http.end();
        return -1;
    }

    http.end();
    return rc;
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

    if (thingName().length() == 0 || deviceID().length() == 0)
    {
        flxLog_E(F("Device/Thing Name not provided. Unable to continue with ArduinoIoT"));
        return false;
    }

    // Create our payload
    DynamicJsonDocument jDoc(80);
    jDoc["device_id"] = deviceID();
    jDoc["name"] = thingName();

    // Create our output
    DynamicJsonDocument jOut(512);

    int rc = postJSONPayload(kArduinoIoTThingsPath, jDoc, jOut);

    switch (rc)
    {
    case 201: // success
        if (jOut.containsKey("id"))
        {
            thingID = jOut["id"].as<const char *>();

            // Okay, we now have a thing ID - let's persist it.
            if (!flxSettings.save(this))
                flxLog_W(F("%s: Error saving Arduino Thing ID"), this->name());

            return true;
        }
        else
            flxLog_E(F(" Arduino IoT Thing Token not returned."));
        break;

    case 412: // exists
        flxLog_E(F("%s: Thing %s already exists. Set the Thing ID property, or delete the existing Cloud Thing named %s and "
                   "restart"), name(), thingName().c_str());
        break;

    default: // fail
        flxLog_E(F("%s: Connection to ArduinoIoT Cloud connection failed. Thing Creation failed"), name());
        break;
    }

    return false;
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


    if (!checkToken())
        return false;

    // do we have a thing ID? We need a "Thing" to attach variables do
    if (thingID().length() == 0)
    {
        if (!createArduinoThing())
        {
            flxLog_E(F("Arduino IoT Cloud not available"));
            return false;
        }
    }

    // Build our payload
    DynamicJsonDocument jDoc(128);

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
    DynamicJsonDocument jOut(700);

    // get the correct path
    char szPath[96];
    snprintf(szPath, sizeof(szPath), "%s/%s/properties", kArduinoIoTThingsPath, thingID().c_str());

    int rc = postJSONPayload(szPath, jDoc, jOut);

    switch (rc)
    {

    case 201: // success

        // get an id?
        if (!jOut.containsKey("id"))
        {
            flxLog_E(F("%s: create variable failed"));
            return false;
        }
        break;

    case 400:
    case 412: // variable already exists - lets use it!
        break;

    default:
        flxLog_E(F("%s: Cloud not available. Unable to create variable %s"), name(), szNameBuffer);
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
    if (!value)
    {
        flxLog_E(F("%s: Unable to update cloud variable. Invalid value."));
        return;
    }

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

    if (!_isEnabled || !_canConnect || !_bInitialized)
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

            // Is this value in our parameter map?
            auto itSearch = _parameterToVar.find(hash_id);

            // No match?
            if (itSearch == _parameterToVar.end())
            {
                flxLog_I(F("%s: Creating Cloud Variable: %s (%s)"), this->name(), szNameBuffer, kvParam.key().c_str());

                // Need to create a ArduinoIoT variable for this parameter
                if (!createArduinoIoTVariable(szNameBuffer, hash_id, dataType))
                    continue;

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

    // Call Arduino cloud update to push new values to the cloud
    if (_bInitialized)
        ArduinoCloud.update();
}

//---------------------------------------------------------------------------------------
///
/// @brief          Framework loop call

bool flxIoTArduino::loop(void)
{
    // Call Arduino update if:
    //  - The system is initalized
    //  - The startup update call counter is below our limit
    //  - Delta is greater than the time limit

    if (_bInitialized && _startupCounter < kArduinoIoTStartupLimit &&
        millis() - _lastArduinoUpdate > kArduinoIoTUpdateDelta)
    {
        ArduinoCloud.update();
        _lastArduinoUpdate = millis();
        _startupCounter++;
    }
    return false;
}
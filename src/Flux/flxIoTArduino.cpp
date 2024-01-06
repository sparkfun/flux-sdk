/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
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

// The First kArduinoIoTStartupLimit calls to Arduino Cloud update sets up the system. So we
// make these calls quickly at start up in the loop method.

#define kArduinoIoTStartupLimit 40

///---------------------------------------------------------------------------------------
///
/// @brief  Called when the network connects
///
void flxIoTArduino::connect(void)
{

    if (deviceID().empty() || deviceSecret().empty())
    {
        flxLog_E(F("%s: Device credentials (ID, Secret) not set - unable to continue"), this->name());
        return;
    }

    if (!_bInitialized)
    {
        ArduinoCloud.setBoardId(deviceID().c_str());
        ArduinoCloud.setSecretDeviceKey(deviceSecret().c_str());

        // begin our session with the ArduinoCloud - pass in our special Connection Handler
        if (!ArduinoCloud.begin(_myConnectionHandler))
        {
            flxLogM_E(kMsgErrInitialization, this->name(), "connection");
            return;
        }
        // Set the ArduinoCloud debug level - it's a function - global - annoying
        setDebugMessageLevel(DBG_ERROR);
        _bInitialized = true;
        _startupCounter = 0;
    }
    flxAddJobToQueue(_theJob);
    _myConnectionHandler.setConnected(true);
}

///---------------------------------------------------------------------------------------
///
/// @brief Called when the network disconnects
///
void flxIoTArduino::disconnect(void)
{
    _myConnectionHandler.setConnected(false);
    flxRemoveJobFromQueue(_theJob);
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
    if (cloudAPISecret().empty() || cloudAPIClientID().empty())
    {
        flxLogM_E(kMsgErrValueNotProvided, this->name(), "API credentials");
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
        flxLogM_E(kMsgErrConnectionFailure, this->name(), szURL);
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
        flxLog_E(F("%s: Communication error [%d] - token request"), this->name(), rc);
        http.end();
        return false;
    }

    // The return value of the token can be large, so use a dynamic json doc - stack based ...
    int retSize = http.getSize();

    // results
    DynamicJsonDocument jDoc(retSize + 100);

    if (deserializeJson(jDoc, http.getString()) != DeserializationError::Ok)
    {
        flxLogM_E(kMsgErrValueError, name(), "token");
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
        flxLogM_E(kMsgErrValueError, name(), "token");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------
///
/// @brief - the Web API uses oauth tokens as a credential. It has an time limit, and needs checking.
///
bool flxIoTArduino::checkToken(void)
{
    // do we have a token?
    if (_arduinoToken.empty() || millis() > _tokenTicks)
    {
        if (!getArduinoToken())
        {
            flxLogM_E(kMsgErrResourceNotAvail, "Arduino IoT Cloud/Account");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------------------
///
/// @brief A general PUT call for JSON payloads - used in this class
///
/// @param URL  The full URL of the endpoint to PUT to
/// @param jDoc[In, Out]  JsonDocument that contains the payload for the PUT call on input, results on output

/// @return -1 on error, HTTP return code otherwise
///
int flxIoTArduino::postJSONPayload(const char *url, JsonDocument &jDoc)
{

    if (!url || strlen(url) < 1)
    {
        flxLogM_E(kMsgErrValueNotProvided, this->name(), "URL");
        return -1;
    }
    if (!createWiFiClient())
        return -1;

    if (_arduinoToken.empty())
    {
        flxLogM_E(kMsgErrValueNotProvided, this->name(), "authentication");
        return -1;
    }

    HTTPClient http;

    char szURL[256];
    snprintf(szURL, sizeof(szURL), "%s:%d/%s", kArduinoIoTAPIServer, kArduinoIoTAPIPort, url + (url[0] == '/' ? 1 : 0));

    if (!http.begin(*_wifiClient, szURL))
    {
        flxLogM_E(kMsgErrConnectionFailure, this->name(), szURL);
        return -1;
    }

    // Prepare the HTTP request
    http.addHeader("Content-Type", "application/json");
    http.setAuthorization(_arduinoToken.c_str());
    http.setAuthorizationType("Bearer");

    char szBuffer[256];

    size_t nWritten = serializeJson(jDoc, szBuffer, sizeof(szBuffer));
    jDoc.clear();

    int rc = -1;

    if (!nWritten)
        flxLog_E(F("%s: Error parsing http request body"), name());
    else
    {
        rc = http.PUT((uint8_t *)szBuffer, nWritten);

        // valid response - we are looking for a 201
        if (rc == 201)
        {
            // flxLog_I("SIZE OF OUTPUT: %d", http.getSize());
            if (deserializeJson(jDoc, http.getString()) != DeserializationError::Ok)
            {
                flxLog_E(F("%s: HTTP response payload is invalid"), name());
                rc = -1;
            }
        }
        else if (rc < 0)
        {
            flxLog_E(F("%s: HTTP error: %s"), name(), http.errorToString(rc).c_str());
            rc = -1;
        }
        else if (http.getSize() > 0)
            deserializeJson(jDoc, http.getString());
    }
    http.end();
    return rc;
}

//---------------------------------------------------------------------------------------
///
/// @brief Try to get Thing ID via the ArduinoCloud class - uses mqtt
///
/// @note If this succeeds, new variables cannot be added to the thing.
///
/// @return true on success, false on failure
//
bool flxIoTArduino::getThingIDFallback(void)
{

    // do we have a thing ID?
    if (!_thingID.empty())
        return true;

    // cloud initialized
    if (!_bInitialized)
        return false;

    // Why to use this
    //   - we have a thing name
    //   - we don't have a thing ID
    //   - A thing of the given name exists
    //
    // If this situation, our HTTP call to connect/build a thing will fail
    // because the thing exists. And that method doesn't return a thing ID
    // which is needed.
    //
    // However, the ArduinoCloud mqtt based system will get the thing ID
    // once it's up and running. Loop over the update call to get ArduinoCloud
    // up and running.

    for (int i = 0; i < 50; i++)
    {
        ArduinoCloud.update();
        delay(100);
    }
    String result = ArduinoCloud.getThingId();
    if (result.length() > 0)
    {
        _thingID = result.c_str();
        _fallbackID = true;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
///
/// @brief  with thingID - make sure it's valid - otherwise difficult to tell
///
///
/// @return true on success, false on failure
///
bool flxIoTArduino::setupArduinoThing(void)
{
    // already setup?
    if (_thingValid)
        return true;

    if (deviceID().empty())
    {
        flxLogM_E(kMsgErrValueNotProvided, this->name(), "Device ID");
        return false;
    }

    // Create our payload - it's a query document.
    // This will add a name, and/or a Thing ID if we have one. The server
    // will use these to either validate a thing exists, or create a new thing.

    DynamicJsonDocument jDoc(512);

    jDoc["across_user_ids"] = false;
    jDoc["show_deleted"] = false;
    jDoc["show_properties"] = false;
    jDoc["device_id"] = deviceID();

    // Name?
    if (!_thingName.empty())
        jDoc["name"] = _thingName;

    // we need a string for the ID that lasts the entire transaction ...
    char szBuffer[132];

    // ID?
    if (!_thingID.empty())
    {
        JsonArray ids = jDoc.createNestedArray("ids");
        strncpy(szBuffer, _thingID.c_str(), sizeof(szBuffer));
        ids.add(szBuffer);
    }

    // send the Thing verify/create request
    int rc = postJSONPayload(kArduinoIoTThingsPath, jDoc);

    // flxLog_I("Verify Thing - RC [%d]", rc);
    // serializeJson(jDoc, Serial);

    // Check results from the query
    switch (rc)
    {
    case 201: // a new thing was created

        if (!jDoc.containsKey("id"))
        {
            flxLogM_E(kMsgErrInitialization, this->name(), "Thing");
            return false;
        }
        _thingID = jDoc["id"].as<const char *>();
        _thingName = jDoc["name"].as<const char *>();
        _thingValid = true;

        break;

    case 412: // The thing exists...

        // If we have a name and an ID, we can continue. If we just have a name,
        // we can get the ID using a fallback method - via ArduinoCloud.

        if (_thingID.empty())
        {
            // get ID using fallback method?
            if (getThingIDFallback())
            {
                // let the user now in fallback mode
                flxLog_N_(F("unable to connect to Thing, using fallback mode ..."));
                _thingValid = true;
            }
            else
            {
                // no ID was obtained - damn - so error mode...
                _hadError = true;
                flxLog_E(F("%s: No Arduino Thing ID provided. Enter ID, delete Thing (%s) on Cloud, or enter new Thing "
                           "Name."),
                         name(), _thingName.c_str());
            }
        }
        else
            _thingValid = true;

        break;

    default: // fail

        // does the result contain any details
        if (jDoc.containsKey("detail"))
        {
            flxLog_E(F("%s: return code=%d, %s"), name(), rc, jDoc["detail"].as<const char *>());
            break;
        }

    case -1: // some other error...
        _hadError = true;
        flxLog_E(F("%s: Thing setup failed"), name());
        break;
    }

    // if the thing is valid, save our state - to persist the latest thing info
    if (_thingValid)
    {
        flxLog_N_(F("using Thing `%s` "), _thingName.c_str());
        // Okay, we now have a thing ID - let's persist it.
        if (!flxSettings.save(this))
            flxLogM_W(kMsgErrSavingProperty, "Arduino IoT Thing ID");
    }
    return _thingValid;
}

///---------------------------------------------------------------------------------------
///
/// @brief  Clear out/free memory with the variable map
///
void flxIoTArduino::freeVariableMap(void)
{

    for (auto it : _parameterToVar)
    {

        if (!it.second)
            continue;

        if (it.second->variable)
        {
            switch (it.second->type)
            {
            case flxTypeUInt:
                delete (CloudUnsignedInt *)it.second->variable;
                break;
            case flxTypeInt:
                delete (CloudInt *)it.second->variable;
                break;
            case flxTypeBool:
                delete (CloudBool *)it.second->variable;
                break;
            case flxTypeFloat:
                delete (CloudFloat *)it.second->variable;
                break;
            case flxTypeString:
                delete (CloudString *)it.second->variable;
                break;
            }
        }
        delete it.second;
    }
    _parameterToVar.clear();
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
        flxLogM_E(kMsgErrCreateFailure, name(), szNameBuffer);
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
        flxLogM_E(kMsgErrCreateFailure, name(), szNameBuffer);
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
    if (!checkToken() || !setupArduinoThing())
        return false;

    // If in Thing  ID fallback mode, where the ID was obtained via mqtt, we  can't create
    // variables - it just fails to connect correctly. But we can link to existing variables.
    //
    // Check if in fallback mode.

    if (!_fallbackID)
    {

        char szVarName[strlen(szNameBuffer) + 1];

        if (!flx_utils::createVariableName(szNameBuffer, szVarName))
        {
            flxLogM_E(kMsgErrCreateFailure, name(), szNameBuffer);
            return false;
        }
        // Build our payload
        DynamicJsonDocument jDoc(704);

        jDoc["name"] = szNameBuffer;       // The friendly name of the property
        jDoc["variable_name"] = szVarName; // The sketch variable name of the property
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
            flxLogM_E(kMsgErrInvalidState, "Arduino IoT - type");
            return false;
        }

        // get the correct path
        char szPath[96];
        snprintf(szPath, sizeof(szPath), "%s/%s/properties", kArduinoIoTThingsPath, _thingID.c_str());

        int rc = postJSONPayload(szPath, jDoc);

        switch (rc)
        {

        case 201: // success

            // get an id?
            if (!jDoc.containsKey("id"))
            {
                flxLogM_E(kMsgErrCreateFailure, name(), "cloud variable");
                return false;
            }
            break;

        case 400:
        case 412: // variable already exists - lets use it!
            break;

        default:
            return false; // error message was sent out by HTTP routine
        }
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
        flxLogM_E(kMsgErrValueError, name(), "cloud variable");
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
/// @brief          Receives a JSON Document that contains the current values to output
/// @note           This method is part of the `flxIWriterJSON` interface
/// @param jDoc     The JSON Document that contains the values to output.
///
void flxIoTArduino::write(JsonDocument &jDoc)
{

    // The Plan:
    // Loop over the objects in the JSON document, each object is a device. For each
    // device, loop over the data values/output parameters and:
    //	- Create a name - 64 char max
    //	- hash name, see if it's in the parameter map object
    //		- Yes, update value
    //		- No - create and register variable in Arduino cloud, then update value

    if (!_isEnabled || !_canConnect || !_bInitialized || _hadError)
        return;

    JsonObject jObj;
    char szNameBuffer[65];
    uint32_t hash_id;
    flxDataType_t dataType;

    bool setupState = false;

    JsonObject jRoot = jDoc.as<JsonObject>(); // needed so we can iterate over object
    for (JsonPair kvObj : jRoot)
    {

        // if this is not an object (for example a timestamp, continue)
        if (!kvObj.value().is<JsonObject>())
            continue;

        // Go through the parameters in the object

        for (JsonPair kvParam : kvObj.value().as<JsonObject>())
        {
            // we need the data type.
            dataType = getValueType(kvParam);

            // make a name
            snprintf(szNameBuffer, sizeof(szNameBuffer), "%s %s", kvObj.key().c_str(), kvParam.key().c_str());

            hash_id = flx_utils::id_hash_string(szNameBuffer); // hash name

            // Is this value in our parameter map?
            auto itSearch = _parameterToVar.find(hash_id);

            // No match?
            if (itSearch == _parameterToVar.end())
            {
                if (!setupState)
                {
                    flxLog_I_(F("%s: setup variables..."), this->name());
                    setupState = true;
                }
                else
                    flxLog_N_(F("."));

                // Need to create a ArduinoIoT variable for this parameter
                if (!createArduinoIoTVariable(szNameBuffer, hash_id, dataType))
                {
                    // from observation, once a variable create fails, the remaining vars will
                    // also fail. Best to break, and try to finish up next write iteration through.
                    setupState = false;
                    return;
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

    if (setupState)
        flxLog_N(F("done"));
}

//---------------------------------------------------------------------------------------
///

void flxIoTArduino::jobUpdateCB(void)
{
    // Call Arduino update if:
    //  - The system is initialized
    //  - our variable map contains variables

    if (_bInitialized && !_parameterToVar.empty())
    {

        // NOTE:
        //  Found that if update() is called before the variables for the Device/Thing
        //  are created and/or connected to, the variable create/connect process triggers
        //  network disconnects (WiFi drops). But, if update isn't called until we have
        //  variables in our parameter map, the system works as expected. So update() isn't
        //  called if the _parameterToVar map is empty.
        //
        // update the Arduino Cloud
        ArduinoCloud.update();

        // inc the startup counter if below limit
        if (_startupCounter < kArduinoIoTStartupLimit)
        {
            _startupCounter++;

            // if at limit, adjust the loop time limit delta
            if (_startupCounter == kArduinoIoTStartupLimit)
            {
                // do we need to update the job period
                if (_theJob.period() != kArduinoIoTStartupLimit * 3)
                    _theJob.setPeriod(kArduinoIoTStartupLimit * 3);
            }
        }
    }
}
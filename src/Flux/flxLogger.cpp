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
 
/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "flxLogger.h"
#include "flxFlux.h"
#include <string.h>
#include <time.h>

flxLogger::flxLogger() : _timestampType{TimeStampNone}, _sampleNumberEnabled{false}, _currentSampleNumber{0}
{
    setName("Logger", "Data logging action");

    flxRegister(timestampMode, "Timestamp Mode", "Enable timestamp output and set the format of a log entry timestamp");

    // Register the timestamp output parameter, then remove it from the action parameter list - we want this hidden
    // from the menu system. We register to connect the parameter to this instance.
    flxRegister(timestamp, "Time");
    removeParameter(timestamp);

    // Sample number things

    flxRegister(numberMode, "Sample Numbering", "An incremental count of the current log entry");
    flxRegister(numberIncrement, "Numbering Increment", "Increment amount for Sample Numbering");

    // Register and remove the output parameter in a simpler manner as the Timestamp
    flxRegister(sampleNumber, "Entry");
    removeParameter(sampleNumber);

    flxRegister(resetSampleNumber, "Reset Sample Counter", "Reset the sample number counter to the provided value");

    _objsToLog.setName("Logger Objects");
    
    flux.add(this);
}
//----------------------------------------------------------------------------
// logScalar()
//
// Outputs the value of a scalar parameter.
//
void flxLogger::logScalar(flxParameterOutScalar *pScalar)
{

    // Key off parameter type, get the correct data value and type, call
    // writeValue(), which will dispatch to "formatter/writers" added
    // to the logger

    switch (pScalar->type())
    {
    case flxTypeBool:
        writeValue(pScalar->name(), pScalar->getBool());
        break;
    case flxTypeInt8:
        writeValue(pScalar->name(), pScalar->getInt8());
        break;
    case flxTypeInt16:
        writeValue(pScalar->name(), pScalar->getInt16());
        break;
    case flxTypeInt:
        writeValue(pScalar->name(), pScalar->getInt());
        break;
    case flxTypeUInt8:
        writeValue(pScalar->name(), pScalar->getUint8());
        break;
    case flxTypeUInt16:
        writeValue(pScalar->name(), pScalar->getUint16());
        break;
    case flxTypeUInt:
        writeValue(pScalar->name(), pScalar->getUint());
        break;
    case flxTypeFloat:
        writeValue(pScalar->name(), pScalar->getFloat(), pScalar->precision());
        break;
    case flxTypeDouble:
        writeValue(pScalar->name(), pScalar->getDouble(), pScalar->precision());
        break;
    case flxTypeString:
        writeValue(pScalar->name(), pScalar->getString());
        break;

    default:
        flxLog_D("Unknown Parameter Value");
        break;
    }
}
//----------------------------------------------------------------------------
// logArray()
//
// Manages the logging of array parameters.

void flxLogger::logArray(flxParameterOutArray *pParam)
{
    // Key off parameter type and call logArrayType<> with required type

    switch (pParam->type())
    {

    case flxTypeBool:
        logArrayType<flxDataArrayBool>(pParam);
        break;

    case flxTypeInt8:
        logArrayType<flxDataArrayInt8>(pParam);
        break;

    case flxTypeInt16:
        logArrayType<flxDataArrayInt16>(pParam);
        break;

    case flxTypeInt:
        logArrayType<flxDataArrayInt>(pParam);
        break;

    case flxTypeUInt8:
        logArrayType<flxDataArrayUint8>(pParam);
        break;

    case flxTypeUInt16:
        logArrayType<flxDataArrayUint16>(pParam);
        break;

    case flxTypeUInt:
        logArrayType<flxDataArrayUint>(pParam);
        break;

    case flxTypeFloat:
        logArrayType<flxDataArrayFloat>(pParam, pParam->precision());
        break;

    case flxTypeDouble:
        logArrayType<flxDataArrayDouble>(pParam, pParam->precision());
        break;

    case flxTypeString:
        logArrayType<flxDataArrayString>(pParam);
        break;

    default:
        flxLog_D("Unknown Array Parameter Value");
        break;
    }
}
//----------------------------------------------------------------------------
// Log the data in a section of the output - title and parameter values
void flxLogger::logSection(const char *section_name, flxParameterOutList &paramList)
{

    if (paramList.size() == 0)
        return;

    for (auto theFormatter : _Formatters)
        theFormatter->beginSection(section_name);

    for (auto param : paramList)
    {
        // Is this parameter enabled?
        if (!param->enabled())
            continue;

        // is this an array or a scalar? Note: using covariant return values to get correct pointer
        if ((param->flags() & kParameterOutFlagArray) == kParameterOutFlagArray)
            logArray((flxParameterOutArray *)param->accessor());
        else
            logScalar((flxParameterOutScalar *)param->accessor());
    }

    for (auto theFormatter : _Formatters)
        theFormatter->endSection();
}
//----------------------------------------------------------------------------
void flxLogger::logObservation(void)
{

    // Begin the observation with all our formatters
    for (auto theFormatter : _Formatters)
        theFormatter->beginObservation();

    // if we have general params to log, do those. This will log to all
    // formatters
    if (_paramsToLog.size() > 0)
        logSection("General", _paramsToLog);

    // loop over objs to log - each object is in a named section. Logs to all
    // formatters
    for (auto pObj : _objsToLog)
        logSection(pObj->name(), pObj->getOutputParameters());

    // And end the observation for each formatter
    for (auto theFormatter : _Formatters)
    {
        theFormatter->endObservation();

        // Write out this observation and clear it out
        theFormatter->writeObservation();
        theFormatter->clearObservation();
    }
}
//----------------------------------------------------------------------------
// log message
//
// Output a general message to the log output. When called, this will leave a text
// blurb in a log stream. 

void flxLogger::logMessage(char * header, char * message)
{
    // Begin the observation with all our formatters
    for (auto theFormatter : _Formatters)
    {
        // clear out any pending information (normally headers)
        theFormatter->reset();

        theFormatter->beginObservation();
        
        theFormatter->beginSection("Message");
        theFormatter->logValue(header, message);
        theFormatter->endSection();

        theFormatter->endObservation();
        theFormatter->writeObservation();
        theFormatter->clearObservation();
        
        theFormatter->reset();        
    }

}
//----------------------------------------------------------------------------
// Timestamp type property get/set
//----------------------------------------------------------------------------
uint flxLogger::get_ts_type(void)
{
    return _timestampType;
}
//----------------------------------------------------------------------------
void flxLogger::set_ts_type(uint newType)
{
    if ((Timestamp_t)newType == _timestampType)
        return;

    // Are we going from having a timestamp to not having a timestamp?
    if ((Timestamp_t)newType == TimeStampNone)
    {
        // Remove the timestamp parameter from our internal timestamp list
        auto iter = std::find(_paramsToLog.begin(), _paramsToLog.end(), &timestamp);

        if (iter != _paramsToLog.end())
            _paramsToLog.erase(iter);
    }
    else if (_timestampType == TimeStampNone)
    {
        // We want the sample number to always the first item in the log line, so make check

        auto iter = _paramsToLog.begin();
        if (!_paramsToLog.empty() && _paramsToLog.front() == &sampleNumber)
            iter++;

        // insert the timestamp parameter at the start of our parameters to log list
        _paramsToLog.insert(iter, &timestamp);
    }

    _timestampType = (Timestamp_t)newType;
}
//----------------------------------------------------------------------------
// Return the current timestamp, as outlined in the timestamp mode.
std::string flxLogger::get_timestamp(void)
{

    char szBuffer[64];

    memset(szBuffer, '\0', sizeof(szBuffer));

    std::string sTitle = "Time";

    time_t t_now;
    time(&t_now);
    struct tm *tmLocal = localtime(&t_now);
    switch (_timestampType)
    {
    case TimeStampMillis:
        snprintf(szBuffer, sizeof(szBuffer), "%lu", millis());
        sTitle += "(millis)";
        break;

    case TimeStampEpoch:
        snprintf(szBuffer, sizeof(szBuffer), "%ld", t_now);
        sTitle += "(Epoch)";
        break;

    case TimeStampDateTimeUSA:
        strftime(szBuffer, sizeof(szBuffer), "%m-%d-%G %T", tmLocal);
        break;

    case TimeStampDateTime:
        strftime(szBuffer, sizeof(szBuffer), "%d-%m-%G %T", tmLocal);
        break;

    case TimeStampISO8601:
    case TimeStampISO8601TZ:
        strftime(szBuffer, sizeof(szBuffer), "%G-%m-%dT%T", tmLocal);

        if (_timestampType == TimeStampISO8601TZ)
        {
            time_t t_gmt = mktime(gmtime(&t_now));
            int deltaT = t_now - t_gmt;

            char chSign;
            if (deltaT < 0)
            {
                chSign = '-';
                deltaT *= -1;
            }else 
                chSign = '+';

            char szTmp[24] = {0};

            int tz_hrs = deltaT / 3600;
            int tz_min = (deltaT % 3600) / 60;

            snprintf(szTmp, sizeof(szTmp), "%c%02d:%02d", chSign, tz_hrs, tz_min);

            strlcat(szBuffer, szTmp, sizeof(szBuffer));
        }
        break;

    case TimeStampNone:
    default:
        break;
    }
    timestamp.setName(sTitle.c_str());
    sTitle = szBuffer;
    return sTitle;
}

//----------------------------------------------------------------------------
// Log sample number property get/set
//----------------------------------------------------------------------------
bool flxLogger::get_num_mode(void)
{
    return _sampleNumberEnabled;
}
//----------------------------------------------------------------------------
void flxLogger::set_num_mode(bool newMode)
{
    if (newMode == _sampleNumberEnabled)
        return;

    // Are we going from having a number to not having a number?
    if (!newMode )
    {
        // Remove the sample number parameter from our internal timestamp list
        auto iter = std::find(_paramsToLog.begin(), _paramsToLog.end(), &sampleNumber);

        if (iter != _paramsToLog.end())
            _paramsToLog.erase(iter);
    }
    else 
    {
        // Add the sample number parameter to our output list. It should be position 0
        _paramsToLog.insert(_paramsToLog.begin(), &sampleNumber);

        // reset sample number on enable.
        reset_sample_number();
    }
    _sampleNumberEnabled = newMode;
}
//----------------------------------------------------------------------------
//
// Reset the sample number to the provided number. Note, the parameter is optional with a
// default value of 0
void flxLogger::reset_sample_number(const uint & number)
{
    _currentSampleNumber = number;
}
//----------------------------------------------------------------------------
// Increment and return the current number
uint flxLogger::get_sample_number(void)
{
    _currentSampleNumber = _currentSampleNumber + numberIncrement();

    return _currentSampleNumber;
}
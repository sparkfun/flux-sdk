/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"
#include "spSpark.h"
#include <string.h>
#include <time.h>

spLogger::spLogger() : _timestampType{TimeStampNone}
{
    setName("Logger", "Data logging action");

    spRegister(timestampMode, "Timestamp Mode", "Enable the output and set the format of a log entry timestamp.");

    // Register the timestamp output parameter, then remove it from the action parameter list - we want this hidden
    // from the menu system. We register to connect the parameter to this instance.
    spRegister(timestamp, "Time");
    removeParameter(timestamp);

    spark.add(this);
}
//----------------------------------------------------------------------------
// logScalar()
//
// Outputs the value of a scalar parameter.
//
void spLogger::logScalar(spParameterOutScalar *pScalar)
{

    // Key off parameter type, get the correct data value and type, call
    // writeValue(), which will dispatch to "formatter/writers" added
    // to the logger

    switch (pScalar->type())
    {
    case spTypeBool:
        writeValue(pScalar->name(), pScalar->getBool());
        break;
    case spTypeInt8:
        writeValue(pScalar->name(), pScalar->getInt8());
        break;
    case spTypeInt16:
        writeValue(pScalar->name(), pScalar->getInt16());
        break;
    case spTypeInt:
        writeValue(pScalar->name(), pScalar->getInt());
        break;
    case spTypeUInt8:
        writeValue(pScalar->name(), pScalar->getUint8());
        break;
    case spTypeUInt16:
        writeValue(pScalar->name(), pScalar->getUint16());
        break;
    case spTypeUInt:
        writeValue(pScalar->name(), pScalar->getUint());
        break;
    case spTypeFloat:
        writeValue(pScalar->name(), pScalar->getFloat(), pScalar->precision());
        break;
    case spTypeDouble:
        writeValue(pScalar->name(), pScalar->getDouble(), pScalar->precision());
        break;
    case spTypeString:
        writeValue(pScalar->name(), pScalar->getString());
        break;

    default:
        spLog_D("Unknown Parameter Value");
        break;
    }
}
//----------------------------------------------------------------------------
// logArray()
//
// Manages the logging of array parameters.

void spLogger::logArray(spParameterOutArray *pParam)
{
    // Key off parameter type and call logArrayType<> with required type

    switch (pParam->type())
    {

    case spTypeBool:
        logArrayType<spDataArrayBool>(pParam);
        break;

    case spTypeInt8:
        logArrayType<spDataArrayInt8>(pParam);
        break;

    case spTypeInt16:
        logArrayType<spDataArrayInt16>(pParam);
        break;

    case spTypeInt:
        logArrayType<spDataArrayInt>(pParam);
        break;

    case spTypeUInt8:
        logArrayType<spDataArrayUint8>(pParam);
        break;

    case spTypeUInt16:
        logArrayType<spDataArrayUint16>(pParam);
        break;

    case spTypeUInt:
        logArrayType<spDataArrayUint>(pParam);
        break;

    case spTypeFloat:
        logArrayType<spDataArrayFloat>(pParam, pParam->precision());
        break;

    case spTypeDouble:
        logArrayType<spDataArrayDouble>(pParam, pParam->precision());
        break;

    case spTypeString:
        logArrayType<spDataArrayString>(pParam);
        break;

    default:
        spLog_D("Unknown Array Parameter Value");
        break;
    }
}
//----------------------------------------------------------------------------
// Log the data in a section of the output - title and parameter values
void spLogger::logSection(const char *section_name, spParameterOutList &paramList)
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
            logArray((spParameterOutArray *)param->accessor());
        else
            logScalar((spParameterOutScalar *)param->accessor());
    }

    for (auto theFormatter : _Formatters)
        theFormatter->endSection();
}
//----------------------------------------------------------------------------
void spLogger::logObservation(void)
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
// Timestamp type property get/set
//----------------------------------------------------------------------------
uint spLogger::get_ts_type(void)
{
    return _timestampType;
}
//----------------------------------------------------------------------------
void spLogger::set_ts_type(uint newType)
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
        // insert the timestamp parameter at the start of our parameters to log list
        _paramsToLog.insert(_paramsToLog.begin(), &timestamp);
    }

    _timestampType = (Timestamp_t)newType;
}
//----------------------------------------------------------------------------
// Return the current timestamp, as outlined in the timestamp mode.
std::string spLogger::get_timestamp(void)
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
            uint32_t deltaT = t_now - t_gmt;

            char szTmp[24] = {0};

            int tz_hrs = deltaT / 3600;
            int tz_min = (deltaT % 3600) / 60;
            if (tz_min < 0)
                tz_min *= -1;

            snprintf(szTmp, sizeof(szTmp), "%c%02d:%02d", tz_hrs < 0 ? '-' : '+', tz_hrs, tz_min);

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
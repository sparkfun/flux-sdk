/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"

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
    // Key off parameter type and do the following
    //  - Get the array object, of the correct type from the parameter
    //     NOTE: This object is allocated off the heap.
    //
    //  - Call writeValue() with the array - this will call the added formatter/output objects
    //  - Delete the array
    //
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

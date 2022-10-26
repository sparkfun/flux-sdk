/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"

//----------------------------------------------------------------------------
void spLogger::logScalar(spParameterOutScalar *pScalar)
{

    switch (pScalar->type()) {
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
void spLogger::logArray( spParameterOutArray * pParam)
{
    switch (pParam->type()) {

    case spTypeBool:
    {
        spDataArrayBool* theArray = (spDataArrayBool*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeInt8:
    {
        spDataArrayInt8* theArray = (spDataArrayInt8*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeInt16:
    {
        spDataArrayInt16* theArray = (spDataArrayInt16*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeInt:
    {
        spDataArrayInt* theArray = (spDataArrayInt*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeUInt8:
    {
        spDataArrayUint8* theArray = (spDataArrayUint8*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeUInt16:
    {
        spDataArrayUint16* theArray = (spDataArrayUint16*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeUInt:
    {
        spDataArrayUint* theArray = (spDataArrayUint*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
    case spTypeFloat:
    {
        spDataArrayFloat* theArray = (spDataArrayFloat*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray, pParam->precision());
            delete theArray;
        }

        break;
    }
    case spTypeDouble:
    {
        spDataArrayDouble* theArray = (spDataArrayDouble*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray, pParam->precision());
            delete theArray;
        }

        break;
    }
    case spTypeString:
    {
        spDataArrayString* theArray = (spDataArrayString*)pParam->get();

        if (theArray != nullptr){
            writeValue(pParam->name(), theArray);
            delete theArray;
        }

        break;
    }
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

        // is this an array or a scalar? 
        if ( (param->flags() & kParameterOutFlagArray ) == kParameterOutFlagArray)
            logArray((spParameterOutArray*)param->accessor());
        else
            logScalar((spParameterOutScalar*)param->accessor());        
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

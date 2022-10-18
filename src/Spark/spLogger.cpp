/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"

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

        switch (param->type())
        {
        case spTypeBool:
            writeValue(param->name(), param->getBool());
            break;
        case spTypeInt8:
            writeValue(param->name(), param->getInt8());
            break;
        case spTypeInt16:
            writeValue(param->name(), param->getInt16());
            break;
        case spTypeInt:
            writeValue(param->name(), param->getInt());
            break;
        case spTypeUInt8:
            writeValue(param->name(), param->getUint8());
            break;
        case spTypeUInt16:
            writeValue(param->name(), param->getUint16());
            break;
        case spTypeUInt:
            writeValue(param->name(), param->getUint());
            break;
        case spTypeFloat:
            writeValue(param->name(), param->getFloat(), param->precision());
            break;
        case spTypeDouble:
            writeValue(param->name(), param->getDouble(), param->precision());
            break;
        case spTypeString:
            writeValue(param->name(), param->getString());
            break;

        default:
            spLog_D("Unknown Parameter Value");
            break;
        }
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

/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"

//----------------------------------------------------------------------------
// When we log a value, we need to write it to all formatters. Seems like a lot
// of short loops, but we want to write the SAME value to all formatters
template <typename T> void spLogger::writeValue(const char *tag, T value)
{

    for (auto theFormatter : _Formatters)
        theFormatter->logValue(tag, value);
}

//----------------------------------------------------------------------------
// Log the data in a section of the output - title and parameter values
void spLogger::logSection(const char *section_name, spDataCoreList &params)
{

    if (params.size() == 0)
        return;

    for (auto theFormatter : _Formatters)
        theFormatter->beginSection(section_name);

    for (auto op : params)
    {

        switch (op->type())
        {
        case TypeBool:
            writeValue(op->name, op->getBool());
            break;
        case TypeInt:
            writeValue(op->name, op->getInt());
            break;

        case TypeFloat:
        case TypeDouble:
            writeValue(op->name, op->getFloat());
            break;

        case TypeString:
            writeValue(op->name, op->getString());
            break;

        default:
            Serial.println("Unknown Parameter Value");
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

    // if we have general params to log, do those. This will log to all formatters
    if (_paramsToLog.size() > 0)
        logSection("General", _paramsToLog);

    // loop over objs to log - each object is in a named section. Logs to all formatters
    for (auto pObj : _objsToLog)
        logSection(pObj->name, pObj->getOutputParameters());

    // And end the observation for each formatter
    for (auto theFormatter : _Formatters)
    {
        theFormatter->endObservation();

        // Write out this observation and clear it out
        theFormatter->writeObservation();
        theFormatter->clearObservation();
    }
}

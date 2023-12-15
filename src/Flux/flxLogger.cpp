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
#include "flxUtils.h"
#include <string.h>
#include <time.h>

//----------------------------------------------------------------------------
// Our time rate/metrics class
//
// This is in a class to make it easy to setup/shutdown...
//----------------------------------------------------------------------------
class _flxLoggerMetrics
{
  public:
    _flxLoggerMetrics()
    {
        // adds move to next entry - so put current at end of list
        _current = kMetricBufferSize - 1;
        memset(_Metrics, '\0', sizeof(_Metrics));
    }
    //----------------------------------------------------------------------------
    ///
    /// @brief - constructor for metric class
    ///
    void captureMetric(void)
    {
        // grab metric - place in next slot
        _current = nextEntry(_current);
        _Metrics[_current] = millis();
    }
    //----------------------------------------------------------------------------
    ///
    /// @brief returns the next entry - just centralize this
    ///
    /// @param  current - the number to base the next value on
    /// @retval The next number to index in the array
    ///
    inline uint32_t nextEntry(uint32_t current)
    {
        return (current + 1) % kMetricBufferSize;
    }

    //----------------------------------------------------------------------------
    ///
    /// @brief - returns the metric - average log intervals
    ///
    float getMetricRate(void)
    {
        // walk our list - total time deltas and return average
        uint32_t iCurr = nextEntry(_current); // start at current+1 -- oldest entry in the list
        uint32_t iNext = nextEntry(iCurr);
        uint32_t sampleTotal = 0;
        uint32_t nSamples = 0;

        for (int i = 0; i < kMetricBufferSize; i++)
        {
            // we have times and current is less than next.
            if (_Metrics[iCurr] != 0 && _Metrics[iNext] != 0 && _Metrics[iCurr] < _Metrics[iNext])
            {
                // add Time Delta
                sampleTotal += (_Metrics[iNext] - _Metrics[iCurr]);
                nSamples++;
            }
            iCurr = iNext;
            iNext = nextEntry(iNext);
        }
        return nSamples == 0 ? 0. : (float)sampleTotal / (float)nSamples;
    }

  private:
    // Our sample/buffer size...
    static constexpr int kMetricBufferSize = 12;

    uint32_t _Metrics[kMetricBufferSize];
    uint32_t _current;
};

//---------------------------------------------------------------------------
// flxLogger Class
//---------------------------------------------------------------------------

flxLogger::flxLogger()
    : _timestampType{TimeStampNone}, _outputDeviceID{false}, _outputLocalName{false}, _sampleNumberEnabled{false},
      _currentSampleNumber{0}, _pMetrics{nullptr}
{
    setName("Logger", "Data logging action");

    flxRegister(timestampMode, "Timestamp Mode", "Enable timestamp output and set the format of a log entry timestamp");

    // Register the timestamp output parameter, then remove it from the action parameter list - we want this hidden
    // from the menu system. We register to connect the parameter to this instance.
    flxRegister(timestamp, "Time");
    removeParameter(timestamp);

    // Sample number things

    flxRegister(numberMode, "Sample Numbering", "Incremental count of the current log entry");
    flxRegister(numberIncrement, "Numbering Increment", "Increment amount for Sample Numbering");

    // Register and remove the output parameter in a simpler manner as the Timestamp
    flxRegister(sampleNumber, "Entry");
    removeParameter(sampleNumber);

    flxRegister(resetSampleNumber, "Reset Sample Counter", "Reset the sample number counter to the provided value");

    // Output the device ID to the log output?
    flxRegister(enableIDOutput, "Output ID", "Include the Board ID in the log output");

    // and the parameter to support the ID?
    flxRegister(getDeviceID, "Board ID");
    removeParameter(getDeviceID); // added on enable of prop

    // Output the device Name/ to the log output?
    flxRegister(enableNameOutput, "Output Name", "Include the Board Name in the log output");

    // and the parameter to support the Name?
    flxRegister(getLocalName, "Board Name");
    removeParameter(getLocalName); // added on enable of prop

    _opsToLog.setName("Logger Objects");

    flxRegister(logRateMetric, "Rate Metric", "Enabled to record the logging rate data");

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

    // loop over ops to log - operations - each object is in a named section. Logs to all
    // formatters
    for (auto pObj : _opsToLog)
    {
        // call execute if the operation needs to run ...
        pObj->execute();
        logSection(pObj->name(), pObj->getOutputParameters());
    }

    // And end the observation for each formatter
    for (auto theFormatter : _Formatters)
    {
        theFormatter->endObservation();

        // Write out this observation and clear it out
        theFormatter->writeObservation();
        theFormatter->clearObservation();
    }

    // capture metric
    if (_pMetrics)
        _pMetrics->captureMetric();
}
//----------------------------------------------------------------------------
// log message
//
// Output a general message to the log output. When called, this will leave a text
// blurb in a log stream.

void flxLogger::logMessage(char *header, char *message)
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
//
// When the timestamp type changes, change the name of the timestamp output parameter
void flxLogger::updateTimeParameterName(void)
{

    char *timeTitle = "Time";
    switch (_timestampType)
    {
    case TimeStampMillis:
        timeTitle = "Time (millis)";
        break;

    case TimeStampEpoch:
        timeTitle = "Time (Epoch)";
        break;
    }
    timestamp.setName(timeTitle);
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

    updateTimeParameterName();
}

//----------------------------------------------------------------------------
// Return the current timestamp, as outlined in the timestamp mode.
std::string flxLogger::get_timestamp(void)
{

    char szBuffer[64];

    memset(szBuffer, '\0', sizeof(szBuffer));

    time_t t_now;
    time(&t_now);
    struct tm *tmLocal = localtime(&t_now);
    switch (_timestampType)
    {
    case TimeStampMillis:
        snprintf(szBuffer, sizeof(szBuffer), "%lu", millis());
        break;

    case TimeStampEpoch:
        snprintf(szBuffer, sizeof(szBuffer), "%ld", t_now);
        break;

    case TimeStampDateTimeUSA:
        strftime(szBuffer, sizeof(szBuffer), "%m-%d-%G %T", tmLocal);
        break;

    case TimeStampDateTime:
        strftime(szBuffer, sizeof(szBuffer), "%d-%m-%G %T", tmLocal);
        break;

    case TimeStampISO8601:
    case TimeStampISO8601TZ:
        flx_utils::timestampISO8601(t_now, szBuffer, sizeof(szBuffer), _timestampType == TimeStampISO8601TZ);
        break;

    case TimeStampNone:
    default:
        break;
    }

    std::string sBuffer = szBuffer;
    return sBuffer;
}

//----------------------------------------------------------------------------
// Device ID methods for output
//----------------------------------------------------------------------------
bool flxLogger::get_id_enable(void)
{
    return _outputDeviceID;
}

//----------------------------------------------------------------------------
void flxLogger::set_id_enable(bool newMode)
{
    if (newMode == _outputDeviceID)
        return;

    // Are we going from having an ID to not having an ID?
    if (!newMode)
    {
        // Remove ID parameter from our internal parameter list
        auto iter = std::find(_paramsToLog.begin(), _paramsToLog.end(), &getDeviceID);

        if (iter != _paramsToLog.end())
            _paramsToLog.erase(iter);
    }
    else
    {
        // We want the ID to be after sample number and timestamp - if they are enabled.
        auto iter = _paramsToLog.begin();
        if (iter != _paramsToLog.end())
        {
            // sample number?
            if (*iter == &sampleNumber)
                iter++;

            if (iter != _paramsToLog.end() && *iter == &timestamp)
                iter++;
        }

        // Add the ID parameter to our output list.
        _paramsToLog.insert(iter, &getDeviceID);
    }
    _outputDeviceID = newMode;
}
//----------------------------------------------------------------------------
std::string flxLogger::get_device_id(void)
{
    std::string sBuffer = flux.deviceId();
    return sBuffer;
}

//----------------------------------------------------------------------------
// Device name methods for output
//----------------------------------------------------------------------------
bool flxLogger::get_name_enable(void)
{
    return _outputLocalName;
}

//----------------------------------------------------------------------------
void flxLogger::set_name_enable(bool newMode)
{
    if (newMode == _outputLocalName)
        return;

    // Are we going from having an ID to not having an ID?
    if (!newMode)
    {
        // Remove ID parameter from our internal parameter list
        auto iter = std::find(_paramsToLog.begin(), _paramsToLog.end(), &getLocalName);

        if (iter != _paramsToLog.end())
            _paramsToLog.erase(iter);
    }
    else
    {
        // We want the name to be after sample number, timestamp and ID - if they are enabled.
        auto iter = _paramsToLog.begin();
        if (iter != _paramsToLog.end())
        {
            // sample number?
            if (*iter == &sampleNumber)
                iter++;

            if (iter != _paramsToLog.end() && *iter == &timestamp)
                iter++;

            if (iter != _paramsToLog.end() && *iter == &getDeviceID)
                iter++;
        }

        // Add the ID parameter to our output list.
        _paramsToLog.insert(iter, &getLocalName);
    }
    _outputLocalName = newMode;
}
//----------------------------------------------------------------------------
std::string flxLogger::get_name(void)
{
    return flux.localName();
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
    if (!newMode)
    {
        // Remove the sample number parameter from our internal parameter list
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
void flxLogger::reset_sample_number(const uint &number)
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

//----------------------------------------------------------------------------
void flxLogger::setEnableLogRate(bool enable)
{
    if (enable)
    {
        if (!_pMetrics)
        {
            _pMetrics = new _flxLoggerMetrics;
            if (!_pMetrics)
                flxLog_E(F("%s: Error initializing metrics"), name());
        }
    }
    else if (_pMetrics)
    {
        delete _pMetrics;
        _pMetrics = nullptr;
    }
}
float flxLogger::getLogRate(void)
{
    return _pMetrics ? _pMetrics->getMetricRate() : 0;
}

/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------------
 * QwiicLog.h
 *
 *
 *
 * May 2020 - KDB
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// #include <ArduinoJson.h>
#include <initializer_list>
#include <vector>

#include "flxFlux.h"
#include "flxOutput.h"

// KDB Testing begin

class _flxLoggerMetrics;

// KDB Testing end

// Define the Logging class
class flxLogger : public flxActionType<flxLogger>
{

  private:
    // Timestamp type property get/set
    uint32_t get_ts_type(void);
    void set_ts_type(uint32_t);

    std::string get_timestamp(void);

    bool get_id_enable(void);
    void set_id_enable(bool);

    std::string get_device_id(void);

    bool get_name_enable(void);
    void set_name_enable(bool);

    std::string get_name(void);

    bool get_num_mode(void);
    void set_num_mode(bool);

    uint32_t get_sample_number(void);

    void reset_sample_number(const uint32_t &number = 0);

  public:
    flxLogger();

    // output a general message
    void logMessage(char *header, char *message);

    // Template trick - use template varargs to allow multiple objs to be
    // added as part of the constructor call. Note, first arg is a writer,
    // object or property to imply some sort of typing to the call. The
    // method calls va_add() to recurse on the arg list.
    //
    // This pattern is used on constructors and add() methods
    template <typename... Args> flxLogger(flxOutputFormat *a1, Args &&...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> flxLogger(flxOutputFormat &a1, Args &&...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> flxLogger(flxOperation *a1, Args &...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> flxLogger(flxOperation &a1, Args &...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> flxLogger(flxParameterOut *a1, Args &...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> flxLogger(flxParameterOut &a1, Args &...args) : flxLogger()
    {
        va_add(a1, args...);
    }

    //----------------------------------------------------------------------------
    void logObservation(void);

    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listen(flxSignalVoid &theEvent)
    {

        // register the logObservation() method on this instance. When an event
        // is triggered, the logObservation method is called
        theEvent.call(this, &flxLogger::logObservation);
    }

    // Used some template magic to support all event types.
    // Need:
    //   - a callback that takes an arg from the signal - any type
    template <typename T> void logObservation(T value)
    {
        // Trigger a log event
        logObservation();
    }
    // And a listen that can take any event type and wire up the callback.
    //
    // Note: Using the defined parameter type of the signal to drive the
    // logObservation template.
    template <typename T> void listen(T &theEvent)
    {
        theEvent.call(this, &flxLogger::logObservation<typename T::value_type>);
    }

    //----------------------------------------------------------------------------
    // Log Event
    //
    // These set of routines are setup to log an actual event. Outputting
    // the value and the name of the object that triggered the event

    void logEvent(flxOperation *theObj)
    {
        // TODO: Probably need to do more here than dump out the value, but
        // for now this works
        writeValue(theObj->name(), "void");
    }

    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listenLogEvent(flxSignalVoid &theEvent, flxOperation *theObj)
    {
        theEvent.call(this, &flxLogger::logEvent, theObj);
    }

    // Used some template magic to support all event types.
    // Need:
    //   - a callback that takes an arg from the signal - any type
    template <typename T> void logEvent(flxOperation *theObj, T value)
    {
        // TODO: Probably need to do more here than dump out the value, but
        // for now this works
        writeValue(theObj->name(), value);
    }
    // And a listen that can take any event type and wire up the callback.
    //
    // Note: Using the defined parameter type of the signal to drive the
    // logObservation template.
    template <typename T> void listenLogEvent(T &theEvent, flxOperation *theObj)
    {
        theEvent.call(this, &flxLogger::logEvent<typename T::value_type>, theObj);
    }

    // Add routines with var args. Allows any combo of writer, param or spBase
    // to be added in one method call to this object.
    //
    // Could template first arg, but wanted some upfront type checking.
    //
    // Note Args&& for pass args by ref. Otherwise, copy constructor is used.
    template <typename... Args> void add(flxOutputFormat *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxOutputFormat &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxOperation &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxOperation *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxParameterOut *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxParameterOut &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxParameterOutList &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxProperty &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxProperty *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(flxPropertyList &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    // small set of removes - for operations

    template <typename... Args> void remove(flxOperation &a1, Args &&...args)
    {
        va_remove(a1, args...);
    }

    template <typename... Args> void remove(flxOperation *a1, Args &&...args)
    {
        va_remove(a1, args...);
    }

    //------------------------------------------------------------
    // for metrics
    void setEnableLogRate(bool enable);
    bool enabledLogRate(void)
    {
        return _pMetrics != nullptr;
    }
    float getLogRate(void);
    //------------------------------------------------------------

    // Enum for timestamp types.
    typedef enum
    {
        TimeStampNone,
        TimeStampMillis,
        TimeStampEpoch,
        TimeStampEpochMillis,
        TimeStampDateTimeUSA,
        TimeStampDateTime,
        TimeStampISO8601,
        TimeStampISO8601TZ,
    } Timestamp_t;

    // Timestamp property

    flxPropertyRWUInt32<flxLogger, &flxLogger::get_ts_type, &flxLogger::set_ts_type> timestampMode = {
        TimeStampNone,
        {{"No Timestamp", TimeStampNone},
         {"Milliseconds since program start", TimeStampMillis},
         {"Seconds since Epoch", TimeStampEpoch},
         {"Milliseconds since Epoch", TimeStampEpochMillis},
         {"Date Time - USA Date format", TimeStampDateTimeUSA},
         {"Date Time", TimeStampDateTime},
         {"ISO8601 Timestamp", TimeStampISO8601},
         {"ISO8601 Timestamp with Time Zone", TimeStampISO8601TZ}}};

    // output parameter for the timestamp
    flxParameterOutString<flxLogger, &flxLogger::get_timestamp> timestamp;

    // output ID to the log
    flxPropertyRWBool<flxLogger, &flxLogger::get_id_enable, &flxLogger::set_id_enable> enableIDOutput = {false};
    flxParameterOutString<flxLogger, &flxLogger::get_device_id> getDeviceID;

    // output Local name to the log
    flxPropertyRWBool<flxLogger, &flxLogger::get_name_enable, &flxLogger::set_name_enable> enableNameOutput = {false};
    flxParameterOutString<flxLogger, &flxLogger::get_name> getLocalName;

    // Sample number - this increments and outputs a number for each sample taken.
    flxPropertyRWBool<flxLogger, &flxLogger::get_num_mode, &flxLogger::set_num_mode> numberMode = {false};

    flxParameterOutUInt32<flxLogger, &flxLogger::get_sample_number> sampleNumber;

    flxPropertyUInt32<flxLogger> numberIncrement = {1, 1, 10000};

    flxParameterInUInt32<flxLogger, &flxLogger::reset_sample_number> resetSampleNumber = {0, 10000};

    // Logger run rate metric collection?
    flxPropertyRWBool<flxLogger, &flxLogger::enabledLogRate, &flxLogger::setEnableLogRate> logRateMetric = {false};

  private:
    void updateTimeParameterName(void);
    // Output devices
    std::vector<flxOutputFormat *> _Formatters;

    // The things we're logging
    flxOperationContainer _opsToLog;
    flxParameterOutList _paramsToLog;
    flxPropertyList _propsToLog;

    void logScalar(flxParameterOutScalar *);
    void logArray(flxParameterOutArray *);

    // Timestamp things
    Timestamp_t _timestampType;

    // output device id?
    bool _outputDeviceID;

    bool _outputLocalName;

    bool _sampleNumberEnabled;
    uint32_t _currentSampleNumber;

    _flxLoggerMetrics *_pMetrics;

    // Templates used to manage array logging based on type.
    //
    // Note - the array object is dynamically allocated.

    template <typename T> void logArrayType(flxParameterOutArray *pParam)
    {
        T *theArray = (T *)pParam->get();

        if (theArray != nullptr)
        {
            writeValue(pParam->name(), theArray);
            delete theArray;
        }
    }

    // same as above, just adding precision support for our float/double types
    template <typename T> void logArrayType(flxParameterOutArray *pParam, uint16_t precision)
    {
        T *theArray = (T *)pParam->get();

        if (theArray != nullptr)
        {
            writeValue(pParam->name(), theArray, precision);
            delete theArray;
        }
    }
    //----------------------------------------------------------------------------
    // When we log a value, we need to write it to all formatters. Seems like a lot
    // of short loops, but we want to write the SAME value to all formatters

    template <typename T> void writeValue(const std::string &tag, T value)
    {
        for (auto theFormatter : _Formatters)
            theFormatter->logValue(tag, value);
    }

    template <typename T> void writeValue(const std::string &tag, T value, uint16_t precision)
    {
        for (auto theFormatter : _Formatters)
            theFormatter->logValue(tag, value, precision);
    }

    void logSection(const char *section_name, flxParameterOutList &params);

    void logSection(const std::string &name, flxParameterOutList &params)
    {
        logSection(name.c_str(), params);
    }

    // vargs management - how to add things recursively.
    //
    // General pattern for the below methods:
    //	   	- Strip off the first item in the list, based on type
    //      - call internal _add() method - which is typed
    //		- Recurse on remaining args
    //      - when arg list is empty, it calls the noop va_add() method.
    //
    void va_add()
    {
    }

    template <typename T, typename... Args> void va_add(T *a1, Args &&...args)
    {
        _add(a1);
        va_add(args...);
    }
    template <typename T, typename... Args> void va_add(T &a1, Args &&...args)
    {
        _add(a1);
        va_add(args...);
    }

    // removes
    void va_remove()
    {
    }

    template <typename T, typename... Args> void va_remove(T *a1, Args &&...args)
    {
        _remove(a1);
        va_remove(args...);
    }
    template <typename T, typename... Args> void va_remove(T &a1, Args &&...args)
    {
        _remove(a1);
        va_remove(args...);
    }
    //----------------------------------------------------------------------------
    // Internal Adds for final object placement add output writers to the logger.
    void _add(flxOutputFormat &writer)
    {
        _Formatters.push_back(&writer);
    }
    void _add(flxOutputFormat *writer)
    {
        if (writer != nullptr)
            _Formatters.push_back(writer);
    }

    void _add(flxOperation &op)
    {
        _add(&op);
    }
    void _add(flxOperation *op)
    {
        if (op != nullptr)
            _opsToLog.push_back(op);
    }

    void _add(flxParameterOut &param)
    {
        _paramsToLog.push_back(&param);
    }
    void _add(flxParameterOut *param)
    {
        if (param != nullptr)
            _paramsToLog.push_back(param);
    }

    void _add(flxParameterOutList &parameterList)
    {
        for (auto param : parameterList)
        {
            _add(param);
        }
    }
    // Internal method to add the contents of a device list
    void _add(flxOperationContainer &deviceList)
    {
        // Only add devices that have output parameters ..
        for (auto device : deviceList)
        {
            if (device->nOutputParameters() > 0)
                _add(device);
        }
    }

    void _add(flxProperty *prop)
    {
        _propsToLog.push_back(prop);
    }
    void _add(flxProperty &prop)
    {
        _add(&prop);
    }
    void _add(flxPropertyList &propList)
    {
        for (auto prop : propList)
        {
            _add(prop);
        }
    }

    // removes

    void _remove(flxOperation &op)
    {
        _remove(&op);
    }
    void _remove(flxOperation *op)
    {
        if (op != nullptr)
            _opsToLog.remove(op);
    }
};

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

#include <ArduinoJson.h>
#include <initializer_list>
#include <vector>

#include "spOutput.h"
#include "spSpark.h"

// Define the QwiicLog class
class spLogger : public spAction2
{

  public:
    spLogger()
    {
        name = "Logger";
    }

    // Template trick - use template varargs to allow multiple objs to be
    // added as part of the constructor call. Note, first arg is a writer,
    // object or property to imply some sort of typing to the call. The
    // method calls va_add() to recurse on the arg list.
    //
    // This pattern is used on constructors and add() methods
    template <typename... Args> spLogger(spOutputFormat *a1, Args &&...args) : spLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> spLogger(spOutputFormat &a1, Args &&...args) : spLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> spLogger(spOperation *a1, Args &...args) : spLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> spLogger(spOperation &a1, Args &...args) : spLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> spLogger(spParameterOut *a1, Args &...args) : spLogger()
    {
        va_add(a1, args...);
    }

    template <typename... Args> spLogger(spParameterOut &a1, Args &...args) : spLogger()
    {
        va_add(a1, args...);
    }

    //----------------------------------------------------------------------------
    void logObservation(void);

    // Used to register the event we want to listen to, which will trigger this
    // activity.
    void listen(spSignalVoid &theEvent)
    {

        // register the logObservation() method on this instance. When an event
        // is triggered, th logObservation method is called
        theEvent.call(this, &spLogger::logObservation);
    }

    // Add routines with var args. Allows any combo of writer, param or spBase
    // to be added in one method call to this object.
    //
    // Could template first arg, but wanted some upfront type checking.
    //
    // Note Args&& for pass args by ref. Otherwise, copy constructor is used.
    template <typename... Args> void add(spOutputFormat *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spOutputFormat &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spOperation &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spOperation *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spParameterOut *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spParameterOut &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spParameterOutList &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spProperty2 &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spProperty2 *a1, Args &&...args)
    {
        va_add(a1, args...);
    }

    template <typename... Args> void add(spProperty2List &a1, Args &&...args)
    {
        va_add(a1, args...);
    }

  private:
    // Output devices
    std::vector<spOutputFormat *> _Formatters;

    // The things we're logging
    spOperationContainer _objsToLog;
    spParameterOutList _paramsToLog;
    spProperty2List _propsToLog;

    template <typename T> void writeValue(const std::string&, T value);
    void logSection(const char *section_name, spParameterOutList &params);

    void logSection(const std::string& name, spParameterOutList &params){
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

    //----------------------------------------------------------------------------
    // Internal Adds for final object placement add output writers to the logger.
    void _add(spOutputFormat &writer)
    {
        _Formatters.push_back(&writer);
    }
    void _add(spOutputFormat *writer)
    {
        if (writer != nullptr)
            _Formatters.push_back(writer);
    }

    void _add(spOperation &op)
    {
        _objsToLog.push_back(&op);
    }
    void _add(spOperation *op)
    {
        if (op != nullptr)
            _objsToLog.push_back(op);
    }

    void _add(spParameterOut &param)
    {
        _paramsToLog.push_back(&param);
    }
    void _add(spParameterOut *param)
    {
        if (param != nullptr)
            _paramsToLog.push_back(param);
    }

    void _add(spParameterOutList &parameterList)
    {
    	for (auto param : parameterList)
    	{
    		_add(param);
    	}
    }
    // Internal method to add the contents of a device list
    void _add(spOperationContainer &deviceList)
    {
        // Only add devices that have output parameters ..
        for (auto device : deviceList)
        {
            if (device->nOutputParameters() > 0)
                _add(device);
        }
    }

    void _add(spProperty2 *prop)
    {
    	_propsToLog.push_back(prop);
    }
    void _add(spProperty2 &prop)
    {
    	_add(&prop);
    }
    void _add(spProperty2List &propList)
    {
    	for (auto prop : propList)
    	{
    		_add(prop);
    	}
    }
};

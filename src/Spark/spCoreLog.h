

#pragma once

// Messaging/logging system for the framework
#include <stdarg.h>
#include "spOutput.h"

#define SP_LOGGING_ENABLED
// Define logging levels

typedef enum {
    spLogNone,
    spLogError,
    spLogWarning,
    spLogInfo,
    spLogVerbose
} spLogLevel_t;


// ----------------------------------------------------------------------------
// spLoggingDriver()
//
// This class actually does the log work. It allows the backend of the logging 
// system to be changed as needed. For example if you want to use a native
// log system.

class spLoggingDriver {

// Very simple interface - just follow a standard printf() format, but with
// log level also passed in
public:
    virtual int logPrintf(const spLogLevel_t level, const char *fmt, va_list args)=0;
};
// ----------------------------------------------------------------------------
// spLoggingDrvDefault  - our Default driver for log output

class spLoggingDrvDefault : public spLoggingDriver
{
public:
    spLoggingDrvDefault() : _wrOutput(nullptr) {}

    int logPrintf(const spLogLevel_t level, const char *fmt, va_list args);

    void setOutput(spWriter &theWriter)
    {
        setOutput(&theWriter);
    }
    void setOutput(spWriter *theWriter)    
    {
        _wrOutput = theWriter;
    }

private:
    // our output device
    spWriter * _wrOutput;   
};
// ----------------------------------------------------------------------------
// main interface for the logging system
class spLogging 
{
public:
    // this is a singleton 
    static spLogging &get(void)
    {
        static spLogging instance;
        return instance;
    }
    //-------------------------------------------------------------------------
    // Delete copy and assignment constructors - b/c this is singleton.
    spLogging(spLogging const &) = delete;
    void operator=(spLogging const &) = delete;

    //-------------------------------------------------------------------------
    void setLogLevel(spLogLevel_t level)
    {
        _logLevel = level;
    }

    //-------------------------------------------------------------------------
    spLogLevel_t logLevel(void)
    {
        return _logLevel;
    }
    //-------------------------------------------------------------------------
    void setLogDriver(spLoggingDriver &theDriver){

        _pLogDriver = &theDriver;
    }

    //-------------------------------------------------------------------------
    // generic log interface
    int logPrintf(const spLogLevel_t level, const char *fmt, ...)
    {
        int retval = 0;
        if ( _pLogDriver && level <= _logLevel && level != spLogNone)
        {
            va_list ap;
            va_start(ap, fmt);
            retval =  _pLogDriver->logPrintf(level, fmt, ap);
            va_end(ap);
        }
        return retval;
    }

private:

    spLogging(): _logLevel{spLogWarning}, _pLogDriver{nullptr}{}

    spLogLevel_t        _logLevel;

    spLoggingDriver   * _pLogDriver;
};
extern spLogging &spLog;


// Define log macros used throughout the system for logging 
#ifdef SP_LOGGING_ENABLED

#define spLog_V(format, ...) spLog.logPrintf(spLogVerbose, format, ##__VA_ARGS__)
#define spLog_I(format, ...) spLog.logPrintf(spLogInfo, format, ##__VA_ARGS__)
#define spLog_W(format, ...) spLog.logPrintf(spLogWarning, format, ##__VA_ARGS__)
#define spLog_E(format, ...) spLog.logPrintf(spLogError, format, ##__VA_ARGS__)

#else

#define spLog_V(format, ...) do {} while(0)
#define spLog_I(format, ...) do {} while(0)
#define spLog_W(format, ...) do {} while(0)
#define spLog_E(format, ...) do {} while(0)

#endif

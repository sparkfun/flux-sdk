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

#pragma once

// Messaging/logging system for the framework
#include "flxCoreEvent.h"
#include <WString.h>
#include <stdarg.h>
#include <vector>

#include "flxCoreInterface.h"
// Lets enable logging
#define SP_LOGGING_ENABLED

// Define logging levels
typedef enum
{
    flxLogNone,
    flxLogError,
    flxLogWarning,
    flxLogInfo,
    flxLogDebug,
    flxLogVerbose
} flxLogLevel_t;

// ----------------------------------------------------------------------------
// flxLoggingDriver()
//
// This class actually does the log work. It allows the back end of the logging
// system to be changed as needed. For example if you want to use a native
// log system.

class flxLoggingDriver
{

    // Very simple interface - just follow a standard printf() format, but with
    // log level also passed in
  public:
    virtual int logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, va_list args) = 0;

    virtual void setLogLevel(flxLogLevel_t level)
    {
    }
};

#ifdef THIS_IS_NOT_WORKING_ON_ESP32
// // ----------------------------------------------------------------------------
// // flxLoggingDrvESP32  -
// //
// // Sends log output to the ESP32 native, IDF system

// class flxLoggingDrvESP32 : public flxLoggingDriver
// {
// public:
//     flxLoggingDrvESP32(){}

//     int logPrintf(const flxLogLevel_t level, const char *fmt, va_list args);

//     void setLogLevel(flxLogLevel_t level);
// private:
//    uint getESPLevel(const flxLogLevel_t level);
// };
#endif
// ----------------------------------------------------------------------------
// flxLoggingDrvDefault  - our Default driver for log output

class flxLoggingDrvDefault : public flxLoggingDriver
{
  public:
    flxLoggingDrvDefault() : _wrOutput(nullptr)
    {
    }

    int logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, va_list args);

    void setOutput(flxWriter &theWriter)
    {
        setOutput(&theWriter);
    }
    void setOutput(flxWriter *theWriter)
    {
        _wrOutput = theWriter;
    }

  private:
    // our output device
    flxWriter *_wrOutput;
};

// ----------------------------------------------------------------------------
// main interface for the logging system
class flxLogging
{
  public:
    // this is a singleton
    static flxLogging &get(void)
    {
        static flxLogging instance;
        return instance;
    }
    //-------------------------------------------------------------------------
    // Delete copy and assignment constructors - b/c this is singleton.
    flxLogging(flxLogging const &) = delete;
    void operator=(flxLogging const &) = delete;

    //-------------------------------------------------------------------------
    void setLogLevel(flxLogLevel_t level)
    {
        _logLevel = level;
        for (auto logDriver : _loggingDrivers)
            logDriver->setLogLevel(level);
    }

    //-------------------------------------------------------------------------
    flxLogLevel_t logLevel(void)
    {
        return _logLevel;
    }
    //-------------------------------------------------------------------------
    void setLogDriver(flxLoggingDriver &theDriver)
    {

        _loggingDrivers.push_back(&theDriver);
        theDriver.setLogLevel(_logLevel);
    }

    //-------------------------------------------------------------------------
    // generic log interface - for flash strings
    int logPrintf(const flxLogLevel_t level, bool newline, const __FlashStringHelper *fmt, ...)
    {
        int retval;

        // pull out var arg list to pass down
        va_list ap;
        va_start(ap, fmt);

        retval = logPrintfInternal(level, newline, reinterpret_cast<const char *>(fmt), ap);

        va_end(ap);

        return retval;
    }
    //-------------------------------------------------------------------------
    // generic log interface
    int logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, ...)
    {

        int retval;

        // pull out var arg list to pass down
        va_list ap;
        va_start(ap, fmt);

        retval = logPrintfInternal(level, newline, fmt, ap);

        va_end(ap);

        return retval;
    }

    // Event to enable subscribe to a message being set. Right now - just for error and warning.
    flxSignalUInt8 onLogMessage;

  private:
    flxLogging() : _logLevel{flxLogWarning}
    {
    }

    //---------------------------------------------------------------------
    // common log driver method - calls our logging drivers
    int logPrintfInternal(const flxLogLevel_t level, bool newline, const char *fmt, va_list ap)
    {
        int retval = 0;
        int tmpval;

        // okay?
        if (_loggingDrivers.size() == 0 || level > _logLevel)
            return retval;

        // loop over  drivers, send out the log info
        for (auto logDriver : _loggingDrivers)
        {
            tmpval = logDriver->logPrintf(level, newline, fmt, ap);
            retval = tmpval | retval; // is this right?
        }

        // trigger an event on error or warning
        if (level == flxLogError || level == flxLogWarning)
            onLogMessage.emit((uint8_t)level);

        return retval;
    }

    // current log level
    flxLogLevel_t _logLevel;

    // list of output drivers used
    std::vector<flxLoggingDriver *> _loggingDrivers;
};

// Our one logging class - easily accessible
extern flxLogging &flxLog;

// Define log macros used throughout the system for logging
#ifdef SP_LOGGING_ENABLED

#define flxLog_V(format, ...) flxLog.logPrintf(flxLogVerbose, true, format, ##__VA_ARGS__)
#define flxLog_D(format, ...) flxLog.logPrintf(flxLogDebug, true, format, ##__VA_ARGS__)
#define flxLog_I(format, ...) flxLog.logPrintf(flxLogInfo, true, format, ##__VA_ARGS__)
#define flxLog_W(format, ...) flxLog.logPrintf(flxLogWarning, true, format, ##__VA_ARGS__)
#define flxLog_E(format, ...) flxLog.logPrintf(flxLogError, true, format, ##__VA_ARGS__)
#define flxLog_N(format, ...) flxLog.logPrintf(flxLogNone, true, format, ##__VA_ARGS__)
#define flxLog__(type, format, ...) flxLog.logPrintf(type, true, format, ##__VA_ARGS__)

// versions what don't end with a newline ...
#define flxLog_V_(format, ...) flxLog.logPrintf(flxLogVerbose, false, format, ##__VA_ARGS__)
#define flxLog_D_(format, ...) flxLog.logPrintf(flxLogDebug, false, format, ##__VA_ARGS__)
#define flxLog_I_(format, ...) flxLog.logPrintf(flxLogInfo, false, format, ##__VA_ARGS__)
#define flxLog_W_(format, ...) flxLog.logPrintf(flxLogWarning, false, format, ##__VA_ARGS__)
#define flxLog_E_(format, ...) flxLog.logPrintf(flxLogError, false, format, ##__VA_ARGS__)
#define flxLog_N_(format, ...) flxLog.logPrintf(flxLogNone, false, format, ##__VA_ARGS__)

#else

#define flxLog_V(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_D(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_I(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_W(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_E(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_N(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog__(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)

// no newline
#define flxLog_V_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_D_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_I_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_W_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_E_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_N_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif

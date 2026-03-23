/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// Messaging/logging system for the framework
// #include "flxCoreEvent.h"
#include <WString.h>
#include <map>
#include <stdarg.h>
#include <stdexcept>
#include <vector>

#include "flxCoreEventID.h"
#include "flxCoreInterface.h"
#include "flxCoreMsg.h"

// Lets enable logging
#define SP_LOGGING_ENABLED

// define our logging error/warning event type
flxDefineEventID(kLogErrWarn);

// Define logging levels
typedef enum
{
    flxLogError = 0,
    flxLogWarning,
    flxLogNone,
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
    int logPrintf(const flxLogLevel_t level, bool newline,
#if defined(ESP32) || defined(ESP8266)
                  const __FlashStringHelper *fmt,
#else
                  const arduino::__FlashStringHelper *fmt,
#endif
                  ...);

    //-------------------------------------------------------------------------
    // generic log interface
    int logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, ...);

    //-------------------------------------------------------------------------
    // generic log interface - takes a message code for the format
    int logPrintf(const flxLogLevel_t level, bool newline, const int idFmt, ...);

    // Add a message block to our log system - pointer version
    void addMessageBlock(const flxMessageBlockCore *theBlock)
    {
        if (theBlock)
            _messageBlocks.push_back(theBlock);
    }

  private:
    flxLogging() : _logLevel{flxLogWarning}
    {
        // add the core message block
        addMessageBlock(&msgBlockFluxCore);
    }

    //---------------------------------------------------------------------
    // common log driver method - calls our logging drivers
    int logPrintfInternal(const flxLogLevel_t level, bool newline, const char *fmt, va_list ap);

    // current log level
    flxLogLevel_t _logLevel;

    // list of output drivers used
    std::vector<flxLoggingDriver *> _loggingDrivers;

    //////////////////////////////////////// Message block testing
    std::vector<const flxMessageBlockCore *> _messageBlocks;
    ////////////////////////////////////////
};

// Our one logging class - easily accessible
extern flxLogging &flxLog;

void flxSetLoggingVerbose(void);
bool flxIsLoggingVerbose(void);
void flxSetLoggingInfo(void);
bool flxIsLoggingInfo(void);
void flxSetLoggingDebug(void);
bool flxIsLoggingDebug(void);

// Define log macros used throughout the system for logging
#ifdef SP_LOGGING_ENABLED

#define flxLog_I(format, ...) flxLog.logPrintf(flxLogInfo, true, format, ##__VA_ARGS__)
#define flxLog_W(format, ...) flxLog.logPrintf(flxLogWarning, true, format, ##__VA_ARGS__)
#define flxLog_E(format, ...) flxLog.logPrintf(flxLogError, true, format, ##__VA_ARGS__)
#define flxLog_N(format, ...) flxLog.logPrintf(flxLogNone, true, format, ##__VA_ARGS__)
#define flxLog__(type, format, ...) flxLog.logPrintf(type, true, format, ##__VA_ARGS__)

// versions what don't end with a newline ...

#define flxLog_I_(format, ...) flxLog.logPrintf(flxLogInfo, false, format, ##__VA_ARGS__)
#define flxLog_W_(format, ...) flxLog.logPrintf(flxLogWarning, false, format, ##__VA_ARGS__)
#define flxLog_E_(format, ...) flxLog.logPrintf(flxLogError, false, format, ##__VA_ARGS__)
#define flxLog_N_(format, ...) flxLog.logPrintf(flxLogNone, false, format, ##__VA_ARGS__)
#define flxLog___(type, format, ...) flxLog.logPrintf(type, false, format, ##__VA_ARGS__)

// versions for message block codes

#define flxLogM_I(format, ...) flxLog.logPrintf(flxLogInfo, true, (const int)format, ##__VA_ARGS__)
#define flxLogM_W(format, ...) flxLog.logPrintf(flxLogWarning, true, (const int)format, ##__VA_ARGS__)
#define flxLogM_E(format, ...) flxLog.logPrintf(flxLogError, true, (const int)format, ##__VA_ARGS__)
#define flxLogM_N(format, ...) flxLog.logPrintf(flxLogNone, true, (const int)format, ##__VA_ARGS__)
#define flxLogM__(type, format, ...) flxLog.logPrintf(type, true, (const int)format, ##__VA_ARGS__)

// versions what don't end with a newline ... but use message block codes

#define flxLogM_I_(format, ...) flxLog.logPrintf(flxLogInfo, false, (const int)format, ##__VA_ARGS__)
#define flxLogM_W_(format, ...) flxLog.logPrintf(flxLogWarning, false, (const int)format, ##__VA_ARGS__)
#define flxLogM_E_(format, ...) flxLog.logPrintf(flxLogError, false, (const int)format, ##__VA_ARGS__)
#define flxLogM_N_(format, ...) flxLog.logPrintf(flxLogNone, false, (const int)format, ##__VA_ARGS__)
#else

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
#define flxLog___(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif

// Define DEBUG log macros used throughout the system for logging
#define SP_DEBUG_LOGGING_ENABLED
#ifdef SP_DEBUG_LOGGING_ENABLED

#define flxLog_V(format, ...) flxLog.logPrintf(flxLogVerbose, true, format, ##__VA_ARGS__)
#define flxLog_D(format, ...) flxLog.logPrintf(flxLogDebug, true, format, ##__VA_ARGS__)
#define flxLog_V_(format, ...) flxLog.logPrintf(flxLogVerbose, false, format, ##__VA_ARGS__)
#define flxLog_D_(format, ...) flxLog.logPrintf(flxLogDebug, false, format, ##__VA_ARGS__)

#define flxLogM_V(format, ...) flxLog.logPrintf(flxLogVerbose, true, (const int)format, ##__VA_ARGS__)
#define flxLogM_D(format, ...) flxLog.logPrintf(flxLogDebug, true, (const int)format, ##__VA_ARGS__)
#define flxLogM_V_(format, ...) flxLog.logPrintf(flxLogVerbose, false, (const int)format, ##__VA_ARGS__)
#define flxLogM_D_(format, ...) flxLog.logPrintf(flxLogDebug, false, (const int)format, ##__VA_ARGS__)
#else

#define flxLog_V(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_D(format, ...)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_V_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLog_D_(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)

#define flxLogM_V(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLogM_D(format, ...)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLogM_V_(format, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#define flxLogM_D_(format, ...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif
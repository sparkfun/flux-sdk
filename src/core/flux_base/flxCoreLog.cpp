/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxCoreLog.h"
#include "flxCoreEvent.h"
#include <string.h>

// Global object - for quick log access
flxLogging &flxLog = flxLogging::get();

#define kOutputBufferSize 96

#define kOutputBracketOpen "["
#define kOutputBracketClose "] "

// Error , Warning, NONE, Info, Debug, Verbose
const char *OutputPrefixCodes[] = {"E", "W", "N", "I", "D", "V"};

//-------------------------------------------------------------------------
// Implement the output for the default driver
//
// Returns the length of the output

int flxLoggingDrvDefault::logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, va_list args)
{

    static char szBuffer[kOutputBufferSize];
    char *pBuffer = szBuffer;

    int lenBuffer = kOutputBufferSize;

    int len;

    va_list copy;

    // no output device, not dice
    if (!_wrOutput)
        return 0;

    // Determine if we have enough room in our string buffer
    // passing in a null dest to vsnprintf will return the number
    // of bytes needed.
    va_copy(copy, args);
    len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);

    // Note: We prefix the message with a level type [E] , which needs 4 chars

    // Do we need to increase the buffer size.
    if (len >= sizeof(szBuffer))
    {
        pBuffer = new char[len + 1];
        // success?
        if (pBuffer == NULL)
            return 0;

        lenBuffer = len + 1;
    }
    // set our prefix if we have a level
    if (level != flxLogNone)
    {
        _wrOutput->write(kOutputBracketOpen, false);

        // colored output?
        if (_wrOutput->colorEnabled())
        {
            if (level == flxLogError)
                _wrOutput->textToRed();
            else if (level == flxLogWarning)
                _wrOutput->textToYellow();
            else if (level == flxLogInfo)
                _wrOutput->textToWhite();
            else if (level == flxLogDebug)
                _wrOutput->textToMagenta();
            else if (level == flxLogVerbose)
                _wrOutput->textToCyan();
        }
        _wrOutput->write(OutputPrefixCodes[level], false);

        // back to normal if in color mode
        if (_wrOutput->colorEnabled())
            _wrOutput->textToNormal();

        _wrOutput->write(kOutputBracketClose, false);
    }

    // Okay,print, staring past prefix
    vsnprintf(pBuffer, len + 1, fmt, args);

    // send to our output device
    _wrOutput->write(pBuffer, newline);

    // free up allocated memory
    if (len >= sizeof(szBuffer))
        delete pBuffer;

    return len;
}

//---------------------------------------------------------------------
// flxLogging Class
//---------------------------------------------------------------------
//
//
//
//-------------------------------------------------------------------------
// generic log interface - for flash strings
int flxLogging::logPrintf(const flxLogLevel_t level, bool newline, const __FlashStringHelper *fmt, ...)
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
int flxLogging::logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, ...)
{

    int retval;

    // pull out var arg list to pass down
    va_list ap;
    va_start(ap, fmt);

    retval = logPrintfInternal(level, newline, fmt, ap);

    va_end(ap);

    return retval;
}
//-------------------------------------------------------------------------
// log interface - but with the format being an message block string ID
int flxLogging::logPrintf(const flxLogLevel_t level, bool newline, const int idFmt, ...)
{
    int retval = 0;

    // do we have this format?
    const char *fmt = nullptr;

    for (auto msgBlock : _messageBlocks)
    {
        fmt = msgBlock->get(idFmt);
        if (fmt != nullptr)
            break;
    }
    if (fmt == nullptr)
        return retval;

    // pull out var arg list to pass down
    va_list ap;
    va_start(ap, idFmt);

    retval = logPrintfInternal(level, newline, fmt, ap);

    va_end(ap);

    return retval;
}
//-------------------------------------------------------------------------
// common log driver method - calls our logging drivers
int flxLogging::logPrintfInternal(const flxLogLevel_t level, bool newline, const char *fmt, va_list ap)
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
        flxSendEvent(flxEvent::kLogErrWarn, (uint8_t)level);

    return retval;
}

#ifdef THIS_IS_NOT_WORKING_ON_ESP32
//-------------------------------------------------------------------------
// ESP32 section
#include <esp_log.h>

const char *kESP32LogTag = "SP";

uint flxLoggingDrvESP32::getESPLevel(const flxLogLevel_t level)
{
    switch (level)
    {
    case flxLogError:
        return ESP_LOG_ERROR;
    case flxLogWarning:
        return ESP_LOG_WARN;
    case flxLogInfo:
        return ESP_LOG_INFO;
    case flxLogDebug:
        return ESP_LOG_DEBUG;
    case flxLogVerbose:
        return ESP_LOG_VERBOSE;
    default:
        break;
    }

    return ESP_LOG_NONE;
}

void flxLoggingDrvESP32::setLogLevel(const flxLogLevel_t level)
{
    // esp_log_level_set(kESP32LogTag, (esp_log_level_t)getESPLevel(level));
    esp_log_level_set("*", ESP_LOG_VERBOSE);
}
//-------------------------------------------------------------------------
// Implement a driver for the ESP32
//
// Sends messages to the IDF/Native system.
//
// Returns the length of the output

int flxLoggingDrvESP32::logPrintf(const flxLogLevel_t level, bool newline, const char *fmt, va_list args)
{
    esp_log_writev((esp_log_level_t)getESPLevel(level), kESP32LogTag, fmt, args);

    return 1;
}
#endif

// Helper function impl

void flxSetLoggingVerbose(void)
{
    flxLog.setLogLevel(flxLogVerbose);
}
bool flxIsLoggingVerbose(void)
{
    return (flxLog.logLevel() == flxLogVerbose);
}
void flxSetLoggingInfo(void)
{
    flxLog.setLogLevel(flxLogInfo);
}
bool flxIsLoggingInfo(void)
{
    return (flxLog.logLevel() >= flxLogInfo);
}
void flxSetLoggingDebug(void)
{
    flxLog.setLogLevel(flxLogDebug);
}
bool flxIsLoggingDebug(void)
{
    return (flxLog.logLevel() >= flxLogDebug);
}
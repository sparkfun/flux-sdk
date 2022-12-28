

#include "flxCoreLog.h"
#include <string.h>

// Global object - for quick log access
flxLogging &flxLog = flxLogging::get();

#define kOutputBufferSize 64

#define kOutputPrefixFMT "[%c] "
#define kOutputPrefixLen 4

// prefix codes - index by log level value
const char OutputPrefixCodes[] = "NEWIDV"; // NONE, Error , Warning, Info, Debug, Verbose

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
    if (len >= sizeof(szBuffer) - kOutputPrefixLen)
    {
        pBuffer = new char[len + 1 + kOutputPrefixLen];
        // success?
        if (pBuffer == NULL)
            return 0;

        lenBuffer = len + 1 + kOutputPrefixLen;
    }
    uint8_t offset = 0;
    // set our prefix if we have a level
    if (level > flxLogNone)
    {
        snprintf(pBuffer, lenBuffer, kOutputPrefixFMT, OutputPrefixCodes[level]);
        offset = kOutputPrefixLen;
    }

    // Okay,print, staring past prefex
    vsnprintf(pBuffer + offset, len + 1, fmt, args);

    // send to our output device
    _wrOutput->write(pBuffer, newline);

    // free up allocated memory
    if (len >= sizeof(szBuffer))
        delete pBuffer;

    return len;
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
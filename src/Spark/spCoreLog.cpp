

#include "spCoreLog.h"
#include <string.h>

// Global object - for quick log access
spLogging &spLog = spLogging::get();

#define kOutputBufferSize 64

#define kOutputPrefixFMT  "[%c] "
#define kOutputPrefixLen  4

// prefix codes - index by log level value
const char  OutputPrefixCodes[] = "NEWIDV"; // NONE, Error , Warning, Info, Debug, Verbose

//-------------------------------------------------------------------------
// Implement the output for the default driver
//
// Returns the length of the output

int spLoggingDrvDefault::logPrintf(const spLogLevel_t level, const char *fmt, va_list args)
{

    static char szBuffer[kOutputBufferSize];
    char *pBuffer = szBuffer;

    int len;
    
    va_list copy;

    // no output device, not dice
    if ( !_wrOutput )
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
        
    }
    // set our prefix
    snprintf(pBuffer, len + 1, kOutputPrefixFMT, OutputPrefixCodes[level]);
    // Okay, now we have room - print and go
    vsnprintf(pBuffer + kOutputPrefixLen, len + 1, fmt, args);

    // send to our output device

    // free up allocated memory
    if (len >= sizeof(szBuffer) )
        delete pBuffer;

    _wrOutput->write(pBuffer);

    return len;
}  
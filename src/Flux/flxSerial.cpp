/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

// Simple impl to support extern of serial
#include "flxSerial.h"

// Global object - for quick access to Serial
flxSerial_ &flxSerial = flxSerial_::get();

void flxSerial_::write(const char *value, bool newline, flxLineType_t type)
{

    // If this is a header, we add a stream indicator to the output
    // start the stream with a Mime Type marker, followed by CR
    if (type == flxLineTypeMime)
    {
        Serial.println();
        Serial.println(value);
        Serial.println();

        // next we'll want to do a header
        _headerWritten = false;
    }
    else if (type == flxLineTypeHeader)
    {
        // only want to write this out once
        if (_headerWritten == false)
        {
            Serial.println(value);
            _headerWritten = true;
        }
    }
    else
    {
        if (newline)
            Serial.println(value);
        else
            Serial.print(value);
    }
}

// Helpful color things
void flxSerial_::textToRed(void)
{
    if (_colorEnabled)
        Serial.print(kClrRed);
}
void flxSerial_::textToGreen(void)
{
    if (_colorEnabled)
        Serial.print(kClrGreen);
}
void flxSerial_::textToYellow(void)
{
    if (_colorEnabled)
        Serial.print(kClrYellow);
}
void flxSerial_::textToBlue(void)
{
    if (_colorEnabled)
        Serial.print(kClrBlue);
}
void flxSerial_::textToWhite(void)
{
    if (_colorEnabled)
        Serial.print(kClrWhite);
}
void flxSerial_::textToNormal(void)
{
    if (_colorEnabled)
        Serial.print(kClrNormal);
}
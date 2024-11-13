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

//
// Defines an observation storage class
//

#pragma once

#include "flxCoreEvent.h"
#include "flxCoreInterface.h"
#include "flxOutput.h"
#include <Arduino.h>

//-----------------------------------------------------------
// Simple serial output class. Making a singleton - this has no state.
//
class flxSerial_ : public flxWriter
{

  public:
    // Singleton things
    static flxSerial_ &get(void)
    {

        static flxSerial_ instance;
        return instance;
    }

    // templated callback for the listener - so we can write out anything
    template <typename T> void write(T value)
    {
        Serial.println(value);
    }

    void write(int value)
    {
        Serial.println(value);
    }
    void write(float value)
    {
        Serial.println(value);
    }
    void write(const char *value, bool newline, flxLineType_t type);

    // Overload listen, so we can type the events, and use the template-based
    // write() method above.

    void listen(flxSignalInt32 &theEvent)
    {
        theEvent.call(this, &flxSerial_::write);
    }
    void listen(flxSignalFloat &theEvent)
    {
        theEvent.call(this, &flxSerial_::write);
    }
    void listen(flxSignalBool &theEvent)
    {
        theEvent.call(this, &flxSerial_::write);
    }
    void listen(flxSignalString &theEvent)
    {
        theEvent.call(this, &flxSerial_::write);
    }
    // copy and assign constructors - delete them to prevent extra copy's being
    // made -- this is a singleton object.
    flxSerial_(flxSerial_ const &) = delete;
    void operator=(flxSerial_ const &) = delete;

    void setColorEnabled(bool bEnable)
    {
        _colorEnabled = bEnable;
    }

    bool colorEnabled(void)
    {
        return _colorEnabled;
    }

    // Helpful color things
    void textToRed(void);
    void textToGreen(void);
    void textToYellow(void);
    void textToBlue(void);
    void textToWhite(void);
    void textToNormal(void);
    void textToCyan(void);
    void textToMagenta(void);

  private:
    flxSerial_() : _headerWritten{false}, _colorEnabled{false} {};

    // Color strings for serial consoles
    static constexpr const char *kClrNormal = "\033[0;39m";
    static constexpr const char *kClrGreen = "\033[1;32m";
    static constexpr const char *kClrYellow = "\033[1;33m";
    static constexpr const char *kClrRed = "\033[1;31m";
    static constexpr const char *kClrBlue = "\033[1;34m";
    static constexpr const char *kClrWhite = "\033[1;37m";
    static constexpr const char *kClrCyan = "\033[1;36m";
    static constexpr const char *kClrMagenta = "\033[1;35m";

    bool _headerWritten;

    bool _colorEnabled;
};

extern flxSerial_ &flxSerial;

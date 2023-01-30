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
 
//
// Defines an observation storage class
//

#pragma once

#include "flxCoreEvent.h"
#include "flxOutput.h"
#include <Arduino.h>

//-----------------------------------------------------------
// Simple serial output class. Making a singleton - this has no state.
//
class flxSerial_ : public flxWriter
{

  public:
    // Singleton things
    static flxSerial_ *getInstance(void)
    {

        static flxSerial_ instance;
        return &instance;
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
    void write(const char *value, bool newline)
    {
        if (newline)
            Serial.println(value);
        else
            Serial.print(value);
    }

    // Overload listen, so we can type the events, and use the templated
    // write() method above.

    void listen(flxSignalInt &theEvent)
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
    // copy and assign constructors - delete them to prevent extra copys being
    // made -- this is a singleton object.
    flxSerial_(flxSerial_ const &) = delete;
    void operator=(flxSerial_ const &) = delete;

  private:
    flxSerial_(){};
};

typedef flxSerial_ *flxSerial;

// Accessor for the signleton
#define flxSerial() flxSerial_::getInstance()

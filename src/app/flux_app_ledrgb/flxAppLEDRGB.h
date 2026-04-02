
/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// quiet a damn pragma message - silly
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "flxAppLED.h"
//---------------------------------------------------------------
class flxAppLEDRGB : public flxAppLEDBase
{
  protected:
    static constexpr const uint8_t kLEDDefaultBrightness = 20;

    void onTimer(void)
    {
        _theLED = _blinkOn ? flxColor::Black : currentState().color;
        FastLED.show();

        _blinkOn = !_blinkOn;
    }

    //------------------------------------------------------------------------------------
    void onUpdate(ledState_t &theState)
    {

        _theLED = currentState().color;
        FastLED.show();
    }

    //------------------------------------------------------------------------------------
    bool onInitialize(uint8_t thePin)
    {
        // Begin setup - turn on board LED during setup.
        pinMode(thePin, OUTPUT);

        FastLED.addLeds<WS2812, thePin, GRB>(&_theLED, 1).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(kLEDDefaultBrightness);

        return true;
    }

  private:
    CRGB _theLED;
};
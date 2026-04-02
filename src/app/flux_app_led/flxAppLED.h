
/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

// Base class for application level / controlled LEDs. This builds out the core
// functionality, defines a base class as well as command/task queue.
//
// Since LED updates can occur rapidly, a FreeROTS queue and task are used to manage
// the posted commands/requests. This also allows event compression.
//
#pragma once

#include <Flux/flxColor.h>
//---------------------------------------------------------------
class flxAppLEDBase
{
  public:
    flxAppLEDBase();

    // Some Handy Flash levels...
    static constexpr uint16_t const kLEDFlashSlow = 600;
    static constexpr uint16_t const kLEDFlashMedium = 200;
    static constexpr uint16_t const kLEDFlashFast = 80;

    // our methods

    bool initialize(uint8_t pin);
    void on(flxColor::color color);
    void off(void);
    void blink(uint32_t);
    void blinkSlow(void)
    {
        blink(kLEDFlashSlow);
    }
    void blinkMedium(void)
    {
        blink(kLEDFlashMedium);
    }
    void blinkFast(void)
    {
        blink(kLEDFlashFast);
    }
    void blink(flxColor::color, uint32_t);
    void blinkSlow(flxColor::color color)
    {
        blink(color, kLEDFlashSlow);
    }
    void blinkMedium(flxColor::color color)
    {
        blink(color, kLEDFlashMedium);
    }
    void blinkFast(flxColor::color color)
    {
        blink(color, kLEDFlashFast);
    }
    void stop(bool off = true);
    void flash(flxColor::color color);
    void refresh(void);

    void setDisabled(bool bDisable);

    bool disabled(void)
    {
        return _disabled;
    }

  protected:
    typedef struct
    {
        flxColor::color color;
        uint32_t ticks;
    } ledState_t;

    // Command Type
    typedef enum
    {
        kCmdNone = 0,
        kCmdOn = (1 << 0),
        kCmdOff = (1 << 1),
        kCmdFlash = (1 << 2),
        kCmdBlink = (1 << 3),
        kCmdReset = (1 << 4),
        kCmdUpdate = (1 << 5)
    } cmdType_t;

    // Command struct
    typedef struct
    {
        cmdType_t type;
        ledState_t data;
        flxAppLEDBase *led;
    } cmdStruct_t;

    ledState_t currentState(void)
    {
        return _ledStack[_current];
    }
    flxColor::color currentColor(void)
    {
        return _ledStack[_current].color;
    }

    // methods the sub-class  implements
    virtual void onTimer(void) = 0;
    virtual void onUpdate(ledState_t &) = 0;
    virtual bool onInitialize(uint8_t) = 0;

    // blink state
    bool _blinkOn;

  private:
    void onEvent(cmdStruct_t &theCommand);
    void update(void);
    void popState(void);
    bool pushState(ledState_t &);
    void queueCommand(cmdType_t command, flxColor::color color = 0, uint32_t ticks = 0);

    // a stack that is used to manage the state of the LED.
    static constexpr const uint16_t kStackSize = 10;
    ledState_t _ledStack[kStackSize];

    int _current;
    bool _isInitialized;

    bool _disabled;

    // FreeRTOS timer
    xTimerHandle _hTimer;
    static constexpr const uint16_t kTimerPeriod = 100;

    // static methods that are used to manage the command queue;
    static void rtosTimerCallback(xTimerHandle pxTimer);
    static void rtosTaskProcessing(void *parameter);
    static bool rtosSetup(void);
};
//------------------------------------------------------------------------------------------
// flxAppLED class
//
// LED - end use implementation for a simple, single color, gpio based LED.
class flxAppLED : public flxAppLEDBase
{
  protected:
    void onTimer(void)
    {
        // toggle blink
        _blinkOn = !_blinkOn;

        digitalWrite(_thePin, _blinkOn ? HIGH : LOW);
    }
    //------------------------------------------------------------------------------------
    void onUpdate(ledState_t &theState)
    {
        // black means off - everything else is on!
        digitalWrite(_thePin, currentColor() == flxColor::Black ? LOW : HIGH);
    }
    //------------------------------------------------------------------------------------
    bool onInitialize(uint8_t thePin)
    {
        // Begin setup - turn on board LED during setup.
        pinMode(thePin, OUTPUT);
        _thePin = thePin;

        return true;
    }

  private:
    uint8_t _thePin;
};

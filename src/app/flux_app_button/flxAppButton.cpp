/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxAppButton.h"

// ISR Callback - C space
static void userButtonISRCallback(void *buttonObj)
{
    if (!buttonObj)
        return;

    // call back into object space
    ((flxAppButton *)buttonObj)->buttonISRCallback();
}
//---------------------------------------------------------------------------
void flxAppButton::buttonISRCallback(void)
{
    if (_theButtonPin != kNoButton)
        _currentEvent = digitalRead(_theButtonPin) == HIGH ? kEventButtonRelease : kEventButtonPress;
}
//---------------------------------------------------------------------------
bool flxAppButton::setupButton(uint8_t theButton)
{
    // okay button value
    if (theButton == kNoButton)
    {
        flxLog_D(F("No board button pin provided. "));
        return false;
    }
    // Anything change and we are running?
    if (_isInitialized)
    {
        if (theButton == _theButtonPin)
            return true;

        if (_theButtonPin != kNoButton)
        {
            detachInterrupt(_theButtonPin);
            _currentEvent = flxAppButton::kEventNoEvent;
        }
    }

    // setup our event handler
    // setup the button
    pinMode(_theButtonPin, INPUT_PULLUP);

    // use interrupt function that takes a parameter (so we can have multiple buttons)
#if defined(ESP32)
    attachInterruptArg(_theButtonPin, userButtonISRCallback, (void *)this, CHANGE);
#elif defined(ARDUINO_ARCH_RP2040)
    attachInterruptArg(_theButtonPin, userButtonISRCallback, CHANGE, (void *)this);
#else
#error "Platform not supported for flxAppButton"
#endif

    _theButtonPin = theButton;

    return true;
}
//---------------------------------------------------------------------------

bool flxAppButton::initialize(void)
{
    // has the button pin set?
    if (!setupButton(_theButtonPin))
        return false;

    _currentEvent = kEventNoEvent;
    // job/timer for when we should check button state
    _jobCheckButton.setup("buttoncheck", 300, this, &flxAppButton::checkButton);
    flxAddJobToQueue(_jobCheckButton);

    _isInitialized = true;

    return true;
}

//---------------------------------------------------------------------------
void flxAppButton::setButtonPin(uint8_t theButton)
{
    if (_isInitialized)
        setupButton(theButton);
    else
        _theButtonPin = theButton;
}
//---------------------------------------------------------------------------

void flxAppButton::checkButton(void)
{
    // Button event / state change?
    if (_currentEvent != flxAppButton::kEventNoEvent)
    {
        if (_currentEvent == flxAppButton::kEventButtonPress)
        {
            _currentInc = 0;
            _incEventTime = millis();
            _pressEventTime = _incEventTime;

            _userButtonPressed = true;

            on_buttonPressed.emit(0);
        }
        else
        {
            _userButtonPressed = false;
            if (millis() - _pressEventTime < 1001)
                on_momentaryPress.emit();
            else
                on_buttonRelease.emit(_currentInc);
        }
        _currentEvent = kEventNoEvent;
    }
    // Is the button pressed ?
    else if (_userButtonPressed)
    {
        // go over the increment time?
        if ((millis() - _incEventTime) / 1000 >= _pressIncrement)
        {
            _currentInc++;
            on_buttonPressed.emit(_currentInc);
            _incEventTime = millis();
        }
    }
}

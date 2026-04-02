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

#include <cstdint>

#include <Flux/flxCore.h>
#include <Flux/flxCoreJobs.h>

// A class to encapsulate the event logic/handling of the on-board button of the DataLogger.
//
// Note: For button press events, the class will send out "increment" events if the button is pressed for a
// longer that "momentary" period

class flxAppButton : public flxActionType<flxAppButton>
{

  public:
    flxAppButton()
        : _theButtonPin{kNoButton}, _pressIncrement{5}, _userButtonPressed{false}, _pressEventTime{0}, _currentInc{0},
          _isInitialized{false}, _currentEvent{kEventNoEvent}
    {
        setName("DataLogger Button", "Manage DataLogger Button Events");
        setHidden();
    }

    bool initialize(void);

    // An Pressed event is sent after each increment value.
    void setPressIncrement(uint inc)
    {
        if (inc > 0)
            _pressIncrement = inc;
    }

    void setButtonPin(uint8_t theButton);
    uint8_t getButtonPin(void) const
    {
        return _theButtonPin;
    }

    void buttonISRCallback(void);

    // Our events - signals ...
    flxSignalVoid on_momentaryPress;
    flxSignalUInt32 on_buttonRelease;
    flxSignalUInt32 on_buttonPressed;

    // probably an issue if we have 255 GPIOs - but okay for now :)
    static constexpr const uint8_t kNoButton = 0xFF;

    // Button Event Types ...

    static constexpr const uint8_t kEventButtonPress = 1;
    static constexpr const uint8_t kEventButtonRelease = 2;
    static constexpr const uint8_t kEventNoEvent = 0xFF;

  private:
    void checkButton(void);
    bool setupButton(uint8_t);

    // the button GPIO bin
    uint8_t _theButtonPin;

    // How many seconds per increment on a button press
    uint32_t _pressIncrement;

    // button pressed?
    bool _userButtonPressed;

    // ticks when the button was pressed
    uint32_t _pressEventTime;

    // Ticks since last increment event
    uint32_t _incEventTime;

    // the current increment count
    uint16_t _currentInc;

    flxJob _jobCheckButton;
    bool _isInitialized;

    volatile uint8_t _currentEvent;
};
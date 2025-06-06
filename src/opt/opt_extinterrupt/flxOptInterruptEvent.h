/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/**
 * @file flxOptInterruptEvent.h
 *
 *  A action to enable an interrupt drive event.
 *
 * When an interrupt is registered/recived, an event is posted.
 */

#pragma once

#include "Arduino.h"
#include "flxCoreEvent.h"
#include "flxCoreJobs.h"
#include "flxFlux.h"

//----------------------------------------------------------------------------------------------------------
// Define our class -

/**
 * @class flxOptExtInterrupt
 */
class flxOptInterruptEvent : public flxActionType<flxOptInterruptEvent>
{

  public:
    /**
     * @brief Default constructor for the flxDevSerial class.
     */
    flxOptInterruptEvent();
    void setEventToSend(flxEvent::flxEventID_t id)
    {
        _eventID = id();
    };

  private:
    // consts
    static constexpr uint8_t kNoPinSet = 255;

    static void the_isr_cb(void);
    // props
    // is enabled?
    bool get_is_enabled(void);
    void set_is_enabled(bool);

    uint16_t get_intr_pin(void);
    void set_intr_pin(uint16_t pin);

    void setupInterrupt(void);
    void shutdownInterrupt(void);
    void jobHandlerCB(void);

    static bool _intr_triggered;

    std::unique_ptr<flxJob> _theJob;
    bool _isEnabled;

    // pin things
    uint16_t _thePin; // interrupt pin number

    flxEvent::flxEventIDNum_t _eventID; // event to send when interrupt occurs         // name of the event to send

    bool _intrSetup;

  public:
    //-----------------------------------------------------
    // methods to set/get PPS pin number
    void setAvailablePins(const uint16_t *pPins, size_t len);

    // enable / disable device ...
    flxPropertyRWBool<flxOptInterruptEvent, &flxOptInterruptEvent::get_is_enabled,
                      &flxOptInterruptEvent::set_is_enabled>
        isEnabled = {false};

    flxPropertyRWUInt16<flxOptInterruptEvent, &flxOptInterruptEvent::get_intr_pin, &flxOptInterruptEvent::set_intr_pin>
        intrPin;

    flxPropertyString<flxOptInterruptEvent> eventName;
};

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
 * @file flxOptInterruptEvent.cpp

 *
 */

#include "Arduino.h"

#include "flxOptInterruptEvent.h"

// update period for the device.

#define kflxInterruptEventUpdateDelta 200
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values,
// and managed properties.

flxOptInterruptEvent::flxOptInterruptEvent()
    : _isEnabled{false}, _thePin{kNoPinSet}, eventName{"Interrupt"}, _intrSetup{false}
{

    // Setup unique identifiers for this device and basic device object systems
    setName("Interrupt Trigger", "Trigger an event from an external interrupt");

    // Register properties
    flxRegister(isEnabled, "Enabled", "When true, this interrupt is enabled");
    flxRegister(eventName, "Event Name", "Name to use for the event");

    flux_add(this);

    setEventToSend(flxEvent::kNoEvent); // No event set by default
}

//---------------------------------------------------------------------------------------
void flxOptInterruptEvent::setAvailablePins(const uint16_t *inPins, size_t len)
{
    if (!inPins || len == 0)
    {
        flxLog_D(F("Interrupt Event Pin list is null or empty"));
        return;
    }

    flxDataLimitSetUInt16 *dataLimit = new flxDataLimitSetUInt16;
    size_t nItems = 0;
    char szBuffer[8];

    // We set the provided pins as limits on the pin property. Let's set that up
    for (int i = 0; i < len; i++)
    {
        if (inPins[i] == 0)
        {
            flxLog_D(F("Interrupt Event Pin list contains a zero pin - ignoring"));
            continue; // skip zero pins
        }
        snprintf(szBuffer, sizeof(szBuffer), "%u", inPins[i]);
        dataLimit->addItem((const char *)szBuffer, inPins[i]);
        nItems++;
        // first pin is our winner
        if (nItems == 1)
            _thePin = inPins[i]; // set the first pin as the default
    }

    if (nItems == 0)
    {
        flxLog_D(F("Interrupt Event Pin list contains no valid pins - ignoring"));
        delete dataLimit; // clean up
        return;           // no valid pins
    }
    intrPin.setDataLimit(dataLimit);

    // Okay, our pin is setup. Make sure the properties are registered.
    if (this->containsProperty(&intrPin) == false)
    {
        flxRegister(intrPin, "Interrupt Pin", "Pin used to send an event");
    }
}
//---------------------------------------------------------------------------------------
bool flxOptInterruptEvent::get_is_enabled(void)
{
    return _isEnabled;
}

void flxOptInterruptEvent::set_is_enabled(bool enabled)
{
    if (enabled == _isEnabled)
        return; // No change, do nothing

    _isEnabled = enabled;

    if (_isEnabled)
        setupInterrupt();
    else
        shutdownInterrupt();
}
//-------------------------------------------------------------
uint16_t flxOptInterruptEvent::get_intr_pin(void)
{
    return _thePin;
}
//-------------------------------------------------------------
void flxOptInterruptEvent::set_intr_pin(uint16_t pin)
{
    if (pin == _thePin)
        return; // no change

    if (pin == kNoPinSet)
    {
        flxLog_D(F("Interrupt pin set to no pin"));
        _thePin = kNoPinSet;
        return;
    }
    // Already setup = lets  disable?
    if (_intrSetup)
        shutdownInterrupt();

    _thePin = pin;

    setupInterrupt();
}
//-------------------------------------------------------------
// our ISR static var for state
bool flxOptInterruptEvent::_intr_triggered = false;

//-------------------------------------------------------------
// ISR Callback for the  interrupt
void flxOptInterruptEvent::the_isr_cb(void)
{
    flxOptInterruptEvent::_intr_triggered = true;
}
//-------------------------------------------------------------
void flxOptInterruptEvent::shutdownInterrupt(void)
{
    if (_intrSetup)
    {
        if (_thePin != kNoPinSet)
            detachInterrupt(_thePin);
        _intrSetup = false;

        // do we have a job setup
        if (_theJob != nullptr)
            flxRemoveJobFromQueue(*_theJob);
    }
}
//-------------------------------------------------------------
void flxOptInterruptEvent::setupInterrupt()
{

    // Everything set?
    if (!_isEnabled || _thePin == kNoPinSet)
    {
        flxLog_E(F("Interrupt Event not enabled or pin not set"));
        return;
    }
    if (_eventID == flxEvent::kNoEvent())
    {
        flxLog_E(F("Interrupt Event Type not set - not enabled"));
        return; // No event to send
    }

    if (_intrSetup)
        return; // Already setup

    // interrupt enabled
    pinMode(_thePin, INPUT);
    attachInterrupt(_thePin, the_isr_cb, RISING);
    _intrSetup = true;
    flxLog_I(F("Interrupt Event Enabled on pin (%u)"), _thePin);

    // If the job is not initialized, create it
    if (_theJob == nullptr)
    {
        _theJob.reset(new flxJob);
        // do we have a job?
        if (_theJob == nullptr)
        {
            flxLog_E("%s: Unable to create job - external serial device disabled", name());
            _isEnabled = false;
            return; // No job allocated
        }
        _theJob->setup(name(), kflxInterruptEventUpdateDelta, this, &flxOptInterruptEvent::jobHandlerCB);
    }
    flxAddJobToQueue(*_theJob);
}

//----------------------------------------------------------------------------------------------------------
// Loop/timer job callback method

void flxOptInterruptEvent::jobHandlerCB(void)
{
    if (!_isEnabled || _eventID == flxEvent::kNoEvent() || _thePin == kNoPinSet || !_intrSetup)
    {
        flxLog_D(F("Interrupt Event job miscconfigured..."));

        // why is the job being called
        flxRemoveJobFromQueue(*_theJob);
        return; // Nothing to do
    }

    if (flxOptInterruptEvent::_intr_triggered == true)
    {
        flxOptInterruptEvent::_intr_triggered = false; // Reset the trigger state
        // Send the event
        flxSendEvent(_eventID, eventName().c_str()); // Send the event
    }
}

/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2026, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "Arduino.h"

#include "flxAppLED.h"

// task handle
static TaskHandle_t hTaskLED = NULL;

// "Command Queue"
static QueueHandle_t hCmdQueue = NULL;

// size of our queue
const uint16_t kLEDCmdQueueSize = 30;
const uint16_t kLEDCmdQueueWait = 5;

// A task needs a Stack - let's set that size
#define kStackSize 1800
#define kActivityDelay 50

//------------------------------------------------------------------------------
// Callback for the FreeRTOS timer -- used to blink LED
// static method

void flxAppLEDBase::rtosTimerCallback(xTimerHandle pxTimer)
{
    // get the user data from the timer - the object to call
    flxAppLEDBase *pLED = static_cast<flxAppLEDBase *>(pvTimerGetTimerID(pxTimer));
    if (pLED)
        pLED->onTimer();
}

//--------------------------------------------------------------------------------
// task event loop - standard C static method - for FreeRTOS
// static method
void flxAppLEDBase::rtosTaskProcessing(void *parameter)
{

    if (hCmdQueue == NULL)
        return;

    // startup delay
    vTaskDelay(50);

    cmdStruct_t theCommand;

    while (true)
    {
        // get next command from the queue - block if needed
        if (xQueueReceive(hCmdQueue, &theCommand, portMAX_DELAY) != pdPASS)
            continue; // should never get here...

        // Flash commands can be compressed - skip dups in queue - if the next command is flash and this
        // command is flash, and the LED is the same skip it.
        if (theCommand.type == kCmdFlash)
        {
            cmdStruct_t thePeek;

            if (xQueuePeek(hCmdQueue, &thePeek, (TickType_t)10))
            {
                // skip the current flash?
                if (thePeek.type == kCmdFlash && theCommand.led == thePeek.led)
                    continue;
            }
        }

        // send the new command to the event processor -- in the object
        if (theCommand.led != NULL)
            theCommand.led->onEvent(theCommand);
    }
}

// static method
bool flxAppLEDBase::rtosSetup(void)
{
    // already setup?
    if (hTaskLED != NULL)
        return true;

    // command queue
    hCmdQueue = xQueueCreate(kLEDCmdQueueSize, sizeof(cmdStruct_t));

    if (hCmdQueue == NULL)
    {
        Serial.println("[ERROR] - LED startup - queue failed");
        return false;
    }

    // Event processing task
    BaseType_t xReturnValue = xTaskCreate(&flxAppLEDBase::rtosTaskProcessing, // Event processing task function
                                          "LEDEventProc",                     // String with name of task.
                                          kStackSize,                         // Stack size in 32 bit words.
                                          NULL,                               // Parameter passed as input of the task
                                          1,                                  // Priority of the task.
                                          &hTaskLED);                         // Task handle.

    if (xReturnValue != pdPASS)
    {
        hTaskLED = NULL;
        Serial.println("[ERROR] - Failure to start event processing task. Halting");
        vQueueDelete(hCmdQueue);
        hCmdQueue = NULL;
        return false;
    }

    // yay - we are rtos'ing
    return true;
}
//---------------------------------------------------------
// core LED implementation
//---------------------------------------------------------

flxAppLEDBase::flxAppLEDBase() : _current{0}, _isInitialized{false}, _blinkOn{false}, _disabled{false}
{
    _ledStack[0] = {flxColor::Black, 0};
}

//---------------------------------------------------------
bool flxAppLEDBase::initialize(uint8_t pin)
{

    // rtos this setup?
    if (!rtosSetup())
        return false;

    // Create a timer, which is used to drive the user experience.
    _hTimer = xTimerCreate("ledtimer", kTimerPeriod / portTICK_RATE_MS, pdTRUE, static_cast<void *>(this),
                           &flxAppLEDBase::rtosTimerCallback);
    if (_hTimer == NULL)
    {
        // no timer - whoa
        Serial.println("[WARNING] - failed to create LED timer");
        return false;
    }

    // call the sub-classed method
    if (!this->onInitialize(pin))
    {
        // okay, cleanup
        xTimerDelete(_hTimer, 100);
        _hTimer = NULL;
        return false;
    }
    _isInitialized = true;

    refresh();

    return true;
}
//---------------------------------------------------------
// Command "event" callback -

void flxAppLEDBase::onEvent(cmdStruct_t &theCommand)
{
    switch (theCommand.type)
    {

    // LED on - new state
    case kCmdOn:
        pushState(theCommand.data);
        break;

    // LED off - pop current state
    case kCmdOff:
        popState();
        break;

    // quick flash of LED
    case kCmdFlash:
        pushState(theCommand.data);
        vTaskDelay(kActivityDelay / portTICK_PERIOD_MS);
        popState();
        break;

    // Change the blink rate
    case kCmdBlink:
        _ledStack[_current].ticks = theCommand.data.ticks;
        update();
        break;

    case kCmdReset:
        _current = 0;
        update();
        break;

    case kCmdUpdate:
        update();
        break;

    case kCmdNone:
    default:
        break;
    }
}
//---------------------------------------------------------
// private.
//---------------------------------------------------------

//---------------------------------------------------------
// Update the LED UX to reflect current state in stack

void flxAppLEDBase::update(void)
{
    if (!_isInitialized)
        return;

    // check blink state - blink on, blink off?
    if (_ledStack[_current].ticks > 0)
    {
        xTimerChangePeriod(_hTimer, _ledStack[_current].ticks / portTICK_PERIOD_MS, 10);
        xTimerReset(_hTimer, 10);
    }
    else
        xTimerStop(_hTimer, 10);

    // call the sub-class onUpdate method
    this->onUpdate(_ledStack[_current]);
}

//---------------------------------------------------------
void flxAppLEDBase::popState(void)
{
    if (_current > 0)
    {
        _current--;
        update();
    }
}
//---------------------------------------------------------
bool flxAppLEDBase::pushState(ledState_t &newState)
{

    if (_current > kStackSize - 2)
        return false; // no room

    // add the new state ..
    _current++;
    _ledStack[_current] = newState;
    update();

    return true;
}

//---------------------------------------------------------
// queue up a command

void flxAppLEDBase::queueCommand(cmdType_t command, flxColor::color color, uint32_t ticks)
{

    if (!_isInitialized)
        return;

    cmdStruct_t theCommand = {command, {color, ticks}, this};

    xQueueSend(hCmdQueue, (void *)&theCommand, 10);
    // if (xQueueSend(hCmdQueue, (void *)&theCommand, 10) != pdPASS)
    // Serial.println("[WARNING] - LED queue overflow"); // TODO -- DO WE CARE
}
//---------------------------------------------------------
// public interface methods.
//---------------------------------------------------------
// Flash the  LED

void flxAppLEDBase::flash(flxColor::color color)
{
    if (_disabled)
        return;

    queueCommand(kCmdFlash, color);
}

//---------------------------------------------------------
// LED Off - end current state

void flxAppLEDBase::off(void)
{
    if (_disabled)
        return;

    queueCommand(kCmdOff);
}

//---------------------------------------------------------
// LED on - new state

void flxAppLEDBase::on(flxColor::color color)
{
    if (_disabled)
        return;

    queueCommand(kCmdOn, color);
}

//---------------------------------------------------------
// Blink - change the timer value of the current color

void flxAppLEDBase::blink(uint32_t timeout)
{
    if (_disabled)
        return;

    queueCommand(kCmdBlink, 0, timeout);
}

//---------------------------------------------------------
// Blink - change state, start blinking

void flxAppLEDBase::blink(flxColor::color color, uint32_t timeout)
{
    if (_disabled)
        return;

    queueCommand(kCmdOn, color, timeout);
}
//---------------------------------------------------------
void flxAppLEDBase::stop(bool turnoff)
{
    if (_disabled)
        return;

    queueCommand(turnoff ? kCmdOff : kCmdBlink);
}
//---------------------------------------------------------
// refresh
void flxAppLEDBase::refresh(void)
{
    if (_disabled)
        return;

    queueCommand(kCmdUpdate);
}

//---------------------------------------------------------
// Enable/Disable?
void flxAppLEDBase::setDisabled(bool bDisable)
{

    if (bDisable == _disabled)
        return;

    _disabled = bDisable;

    // reset???
    if (bDisable)
        queueCommand(kCmdReset);
}

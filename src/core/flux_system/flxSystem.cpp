/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#include "flxSystem.h"
#include "flxPlatform.h"

const char chCR = 13; // for display erase during progress

//-----------------------------------------------------------------------------------
void flxSystem::restartDevicePrompt()
{

    if (!_pSerialSettings)
    {
        flxLogM_E(kMsgErrInitialization, name(), "Settings Interface");
        return;
    }

    // Need to prompt for an a-okay ...
    flxLog_N_(F("\n\r\tPerform Device Restart? [Y/n]? "));
    // Serial.printf("\n\r\tPerform Device Restart? [Y/n]? ");

    uint8_t selected = _pSerialSettings->getMenuSelectionYN();

    flxLog_N("\n\r");

    if (selected != 'y' || selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
    {
        // Serial.printf("\tAborting restart\n\r\r");
        flxLog_N(F("\tAborting restart"));

        return;
    }

    restartDevice();
}

//-----------------------------------------------------------------------------------
void flxSystem::restartDevice()
{
    // Now
    flxLog_I(F("Restarting the device..."));

    flxSendEvent(flxEvent::kOnSystemRestart);
    delay(200);

    flxPlatform::restart_device();
}

//-----------------------------------------------------------------------------------
void flxSystem::resetDevicePrompt()
{

    if (!_pSerialSettings)
    {
        flxLogM_E(kMsgErrInitialization, name(), "Settings Interface");
        return;
    }

    // Need to prompt for an a-okay ...
    flxLog_N_(F("\n\r\tPerform Device Reset? [Y/n]? "));
    // Serial.printf("\n\r\tPerform Device Restart? [Y/n]? ");

    uint8_t selected = _pSerialSettings->getMenuSelectionYN();

    flxLog_N("\n\r");

    if (selected != 'y' || selected == kReadBufferTimeoutExpired || selected == kReadBufferExit)
    {
        // Serial.printf("\tAborting restart\n\r\r");
        flxLog_N(F("\tAborting Reset"));

        return;
    }

    resetDevice();
}

//-----------------------------------------------------------------------------------
void flxSystem::resetDevice()
{
    flxLog_I(F("Resetting the device..."));
    flxSendEvent(flxEvent::kOnSystemReset);
    delay(200);
    restartDevice();
}

/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

// Action to manage system control interactions

#pragma once

#include "flxCore.h"
#include "flxFlux.h"
#include "flxPlatform.h"
#include "flxSettingsSerial.h"

// Define the firmware load event
flxDefineEventID(kOnSystemRestart);
flxDefineEventID(kOnSystemReset);

class flxSystem : public flxActionType<flxSystem>
{

  public:
    flxSystem() : _pSerialSettings{nullptr}
    {

        // Set name and description
        setName("System Control", "System and Device control");

        flxRegister(deviceRestart, "Device Restart", "Restart/reboot the device");
        deviceRestart.prompt = false;

        flxRegister(deviceResetAndRestart, "Device Reset", "Erase all settings and restart");
        deviceResetAndRestart.prompt = false;

        flux_add(this);
    }

    // called to restart the device
    void restartDevicePrompt();
    void restartDevice();
    void resetDevice(void);
    void resetDevicePrompt(void);

    void setSerialSettings(flxSettingsSerial *pSettings)
    {
        _pSerialSettings = pSettings;
    }
    void setSerialSettings(flxSettingsSerial &serSettings)
    {
        setSerialSettings(&serSettings);
    }

    // Our input parameters/functions
    flxParameterInVoid<flxSystem, &flxSystem::restartDevicePrompt> deviceRestart;

    flxParameterInVoid<flxSystem, &flxSystem::resetDevicePrompt> deviceResetAndRestart;

  private:
    flxSettingsSerial *_pSerialSettings;
};

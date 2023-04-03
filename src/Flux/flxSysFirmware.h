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
 

// Action to mange the device/system Firmware - perform a factory reset, apply firmware updates

#pragma once

#include "flxCore.h"
#include "flxFlux.h"

class flxSysFirmware : public flxActionType<flxSysFirmware>
{

  private:

    void doFactoryReset(void);

    //------------------------------------------------------------------------------
    void factory_reset(const bool &doReset)
    {
        if (doReset)
            doFactoryReset();
    };

public:
    flxSysFirmware()
    {

        // Set name and description
        setName("System", "Reset and Update Options");

        flxRegister(factoryReset, "Factory Reset", "Factory reset the device - enter 1 to perform the reset");
        
        flux.add(this);
    }
    
    // Our input parameters/functions
    flxParameterInBool<flxSysFirmware, &flxSysFirmware::factory_reset> factoryReset;
    
};

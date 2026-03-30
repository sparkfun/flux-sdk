
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

#include "flxFLux.h"

// Define our application class interface.
class flxApplication : public flxActionType<flxApplication>
{

  public:
    flxApplication()
    {
        flux.setApplication(this);

        // Set a name/descriptor that shows up in the menu system.
        ((flxObject *)this)->setName("Application Settings", "Main Application Settings");
    }

    // Name things - the overall base object of the system has a descriptor - we want this
    // method signature to name the app, but also allow this app object (which is an action)
    // to be used in the menuing system ... so
    //          - override the flxDescriptor methods on the app object
    //              -- use a descriptor instance variable to store the data
    //          - Methods called on the application will get these name/desc values
    //          - Methods called on sub-class objects get the base object name/desc
    // So we can use the same methods is both cases, but leverage the fact that
    // the base descriptor class methods are not virtual.

    void setName(const char *name)
    {
        appDesc.setName(name);
    }
    void setDescription(const char *desc)
    {
        appDesc.setDescription(desc);
    }
    void setName(const char *name, const char *desc)
    {
        this->setName(name);
        this->setDescription(desc);
    }
    const char *name(void)
    {
        return appDesc.name();
    }

    const char *description(void)
    {
        return appDesc.description();
    }

    // Method is called before anything - allows pre system setup - for example I/O init.
    virtual void onInit(void)
    {
        return;
    }
    // Method is called before device auto-load, settings restoration and action initialization
    virtual bool onSetup(void)
    {
        return true;
    }

    // Called after everything is loaded, restored and initialize
    virtual bool onStart(void)
    {
        return true;
    }

    // called after device auto-load occurs, but before system state restore is called during
    // startup
    virtual void onDeviceLoad(void)
    {
        return;
    }

    // called just prior to system restore on startup
    virtual void onRestore(void)
    {
        return;
    }

    void setVersion(uint major, uint minor, uint point, const char *desc, uint32_t build)
    {
        flux.setVersion(major, minor, point, desc, build);
    }

    void setAppClassID(const char *ID, char prefix[5])
    {
        flux.setAppClassID(ID, prefix);
    }

    virtual bool loop(void)
    {
        return false;
    }

    //   private:
    flxDescriptor appDesc;
};

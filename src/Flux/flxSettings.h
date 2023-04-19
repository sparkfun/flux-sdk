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
 

// Action to control save of settings ...
#pragma once

#include "flxCore.h"
#include "flxFlux.h"
#include "flxStorage.h"

class flxSettingsSave : public flxActionType<flxSettingsSave>
{

  private:

    //------------------------------------------------------------------------------
    void save_settings(void)
    {
        saveSystem();
    };

    //------------------------------------------------------------------------------
    void restore_settings(void)
    {
        restoreSystem();
    };

    void restore_fallback(void);
    void save_fallback(void);    

    void saveEvent_CB(void)
    {
        // Enabled?
        if (saveOnEvent())
        {
            flxLog_I(F("Saving System Settings."));
            save_settings();
        }
    }
    void restoreEvent_CB(void)
    {
        // Enabled?
        if (restoreOnEvent())
            restore_settings();
    }

  public:
    // flxSettingsSave is a singleton
    static flxSettingsSave &get(void)
    {

        static flxSettingsSave instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    flxSettingsSave(flxSettingsSave const &) = delete;
    void operator=(flxSettingsSave const &) = delete;
    
    
    //------------------------------------------------------------
    // settings/storage things. 
    //------------------------------------------------------------
    //
    // Storage device for settings - set this in the system.     

    void setStorage(flxStorage *pStorage);
    void setFallback(flxStorage *pStorage);

    // Save settings for the system
    bool saveSystem(void);

    // save a specific object
    bool save(flxObject &theObject);
    bool save(flxObject *pObject);

    // Restore settings - if no parameter is passed in, the entire system is restored
    bool restoreSystem(void);

    // restore a specific object
    bool restore(flxObject &theObject);
    bool restore(flxObject *pObject);
    void reset(void);


    bool isAvailable()
    {
        return _primaryStorage != nullptr;
    }

    //------------------------------------------------------------------------------
    // Slots for signals - Enables saving and restoring settings base on events
    void listenForSave(flxSignalVoid &theEvent);
    void listenForRestore(flxSignalVoid &theEvent);

    // Properties.
    flxPropertyHiddenBool<flxSettingsSave> saveOnEvent = {true};
    flxPropertyHiddenBool<flxSettingsSave> restoreOnEvent = {true};

    flxPropertyBool<flxSettingsSave> fallbackSave = {false};    
    flxPropertyBool<flxSettingsSave> fallbackRestore = {true};

    // Our input parameters
    flxParameterInVoid<flxSettingsSave, &flxSettingsSave::save_settings> saveSettings;
    flxParameterInVoid<flxSettingsSave, &flxSettingsSave::restore_settings> restoreSettings;
    flxParameterInVoid<flxSettingsSave, &flxSettingsSave::reset> clearSettings;

    flxParameterInVoid<flxSettingsSave, &flxSettingsSave::restore_fallback> restoreFallback;
    flxParameterInVoid<flxSettingsSave, &flxSettingsSave::save_fallback> saveFallback;    


private:

    flxSettingsSave() : _primaryStorage{nullptr}, _fallbackStorage{nullptr}
    {

        // Set name and description
        setName("Save Settings", "Save, Restore and Reset System settings.");

        flxRegister(saveOnEvent, "Save Events", "Save settings on save system events");
        flxRegister(restoreOnEvent, "Restore Events", "Restore settings on restore esystem vents");

        flxRegister(fallbackRestore, "Fallback Restore", "If unable to restore settings, use the fallback source");
        flxRegister(fallbackSave, "Fallback Save", "Save settings also saves to the fallback storage");        


        flxRegister(saveSettings, "Save Settings", "Save current settings to persistent storage");
        flxRegister(restoreSettings, "Restore Settings", "Restore saved settings");
        flxRegister(clearSettings, "Clear Settings", "Erase the settings saved on this device");

        flxRegister(saveFallback, "Save to Fallback", "Save system settings to the fallback storage");                
        flxRegister(restoreFallback, "Restore from Fallback", "Restore system settings from the fallback storage");        


        flux.add(this);
    }

    bool saveObjectToStorage(flxObject*, flxStorage *);
    bool restoreObjectFromStorage(flxObject*, flxStorage *);    

    flxStorage * _primaryStorage;
    flxStorage * _fallbackStorage;
};
extern flxSettingsSave &flxSettings;
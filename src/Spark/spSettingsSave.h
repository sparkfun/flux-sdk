

// Action to control save of settings ...
#pragma once

#include "spCore.h"
#include "spSpark.h"
#include "spStorage.h"

class spSettingsSave : public spActionType<spSettingsSave>
{

  private:

    //------------------------------------------------------------------------------
    void save_settings(void)
    {
        spark.saveSettings();
    };

    //------------------------------------------------------------------------------
    void restore_settings(void)
    {
        spark.restoreSettings();
    };

    //------------------------------------------------------------------------------
    void clear_settings(void)
    {
   
         spark.resetSettings();
    }

    void saveEvent_CB(void)
    {
        // Enabled?
        if (saveOnEvent())
            save_settings();
    }
    void restoreEvent_CB(void)
    {
        // Enabled?
        if (restoreOnEvent())
            restore_settings();
    }

  public:
    spSettingsSave() 
    {

        // Set name and description
        setName("Save Settings", "Save, Restore and Reset System settings.");

        spRegister(saveOnEvent, "Save Events", "Save settings on save events.");
        saveOnEvent = true;
        spRegister(restoreOnEvent, "Restore Events", "Restore settings on restore events.");
        restoreOnEvent = true;

        spRegister(saveSettings, "Save Settings", "Save current settings to persistent storage.");
        spRegister(restoreSettings, "Restore Settings", "Restore saved settings.");
        spRegister(clearSettings, "Clear Settings", "Erase saved settings.");
    }
    
    //------------------------------------------------------------------------------
    // Slots for signals - Enables saving and restoring settings base on events
    void listenForSave(spSignalVoid &theEvent)
    {
        theEvent.call(this, &spSettingsSave::saveEvent_CB);
    };
    void listenForRestore(spSignalVoid &theEvent)
    {
        theEvent.call(this, &spSettingsSave::restoreEvent_CB);
    };

    // Properties.
    spPropertyBool<spSettingsSave> saveOnEvent;
    spPropertyBool<spSettingsSave> restoreOnEvent;

    // Our input parameters
    spParameterInVoid<spSettingsSave, &spSettingsSave::save_settings> saveSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::restore_settings> restoreSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::clear_settings> clearSettings;
};

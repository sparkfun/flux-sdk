

// Action to control save of settings ...
#pragma once

#include "spCore.h"
#include "spSpark.h"
#include "spStorage.h"

class spSettingsSave : public spActionType<spSettingsSave>
{

  private:
    spStorage2 *_saveDest;

    //------------------------------------------------------------------------------
    void save_settings(void)
    {
        if (!_saveDest)
        {
            spLog_E("Save Settings - no destination provided.");
            return;
        }
        spark.save(_saveDest);
    };

    //------------------------------------------------------------------------------
    void restore_settings(void)
    {
        if (!_saveDest)
        {
            spLog_E("Restore Settings - no destination provided.");
            return;
        }
        spark.restore(_saveDest);
    };

    //------------------------------------------------------------------------------
    void clear_settings(void)
    {
        if (!_saveDest)
        {
            spLog_E("Reset Settings - no destination provided.");
            return;
        }
        _saveDest->resetStorage();
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
    spSettingsSave() : _saveDest{nullptr}
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
    spSettingsSave(spStorage2 *theDevice) : spSettingsSave()
    {
        setSaveDestination(theDevice);
    }
    //------------------------------------------------------------------------------
    spSettingsSave(spStorage2 &theDevice) : spSettingsSave(&theDevice)
    {
    }
    //------------------------------------------------------------------------------
    // Set up destination.
    void setSaveDestination(spStorage2 *theDevice)
    {
        _saveDest = theDevice;
    }
    //------------------------------------------------------------------------------
    void setSaveDestination(spStorage2 &theDevice)
    {
        setSaveDestination(&theDevice);
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

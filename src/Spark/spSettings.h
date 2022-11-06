

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
        saveSettings();
    };

    //------------------------------------------------------------------------------
    void restore_settings(void)
    {
        restoreSettings();
    };

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
    // spSettingsSave is a singleton
    static spSettingsSave &get(void)
    {

        static spSettingsSave instance;
        return instance;
    }
    // This is a singleton class - so delete copy & assignment constructors
    spSettingsSave(spSettingsSave const &) = delete;
    void operator=(spSettingsSave const &) = delete;
    
    
    //------------------------------------------------------------
    // settings/storage things. 
    //------------------------------------------------------------
    //
    // Storage device for settings - set this in the system.     

    void setStorage(spStorage &theStorage);
    void setStorage(spStorage *pStorage);

    // Save settings - if no parameter is passed in, the entire system is saved
    bool saveSystem(void);

    // save a specific object
    bool save(spObject &theObject);
    bool save(spObject *pObject);

    // Restore settings - if no parameter is passed in, the entire system is restored
    bool restoreSystem(void);

    // restore a specific object
    bool restore(spObject &theObject);
    bool restore(spObject *pObject);
    void reset(void);

    //------------------------------------------------------------------------------
    // Slots for signals - Enables saving and restoring settings base on events
    void listenForSave(spSignalVoid &theEvent);
    void listenForRestore(spSignalVoid &theEvent);

    // Properties.
    spPropertyBool<spSettingsSave> saveOnEvent;
    spPropertyBool<spSettingsSave> restoreOnEvent;

    // Our input parameters
    spParameterInVoid<spSettingsSave, &spSettingsSave::save_settings> saveSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::restore_settings> restoreSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::reset> clearSettings;

private:

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

        spark.add(this);
    }

    spStorage *  _settingsStorage;
};
extern spSettingsSave &spSettings;
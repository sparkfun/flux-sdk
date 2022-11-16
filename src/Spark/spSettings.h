

// Action to control save of settings ...
#pragma once

#include "spCore.h"
#include "spSpark.h"
#include "spStorage.h"

#include <vector>

class spSettingsSave : public spActionType<spSettingsSave>
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

    void saveEvent_CB(void)
    {
        // Enabled?
        if (saveOnEvent())
        {
            spLog_I(F("Saving System Settings."));
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

    void addStorage(spStorage *pStorage)
    {
        if (pStorage)
            _vStorage.push_back(pStorage);
    }

    // Save settings for the system
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


    bool isAvailable()
    {
        return _settingsStorage != nullptr;
    }

    //------------------------------------------------------------------------------
    // Slots for signals - Enables saving and restoring settings base on events
    void listenForSave(spSignalVoid &theEvent);
    void listenForRestore(spSignalVoid &theEvent);

    // Properties.
    spPropertyBool<spSettingsSave> saveOnEvent = {true};
    spPropertyBool<spSettingsSave> restoreOnEvent = {true};
    spPropertyBool<spSettingsSave> useSecondarySources = {true};

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
        spRegister(restoreOnEvent, "Restore Events", "Restore settings on restore events.");

        spRegister(useSecondarySources, "All Devices", "Use all available devices to store settings");

        spRegister(saveSettings, "Save Settings", "Save current settings to persistent storage.");
        spRegister(restoreSettings, "Restore Settings", "Restore saved settings.");
        spRegister(clearSettings, "Clear Settings", "Erase saved settings.");

        spark.add(this);
    }

    spStorage *  _settingsStorage;

    std::vector<spStorage*> _vStorage;
};
extern spSettingsSave &spSettings;
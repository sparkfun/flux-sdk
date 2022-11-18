

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

    void setStorage(spStorage *pStorage);
    void setFallback(spStorage *pStorage);

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
        return _primaryStorage != nullptr;
    }

    //------------------------------------------------------------------------------
    // Slots for signals - Enables saving and restoring settings base on events
    void listenForSave(spSignalVoid &theEvent);
    void listenForRestore(spSignalVoid &theEvent);

    // Properties.
    spPropertyBool<spSettingsSave> saveOnEvent = {true};
    spPropertyBool<spSettingsSave> restoreOnEvent = {true};

    spPropertyBool<spSettingsSave> fallbackSave = {false};    
    spPropertyBool<spSettingsSave> fallbackRestore = {true};

    // Our input parameters
    spParameterInVoid<spSettingsSave, &spSettingsSave::save_settings> saveSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::restore_settings> restoreSettings;
    spParameterInVoid<spSettingsSave, &spSettingsSave::reset> clearSettings;

    spParameterInVoid<spSettingsSave, &spSettingsSave::restore_fallback> restoreFallback;
    spParameterInVoid<spSettingsSave, &spSettingsSave::save_fallback> saveFallback;    


private:

    spSettingsSave() : _primaryStorage{nullptr}, _fallbackStorage{nullptr}
    {

        // Set name and description
        setName("Save Settings", "Save, Restore and Reset System settings.");

        spRegister(saveOnEvent, "Save Events", "Save settings on save events.");
        spRegister(restoreOnEvent, "Restore Events", "Restore settings on restore events.");

        spRegister(fallbackRestore, "Fallback Restore", "If unable to restore settings, use the fallback source.");
        spRegister(fallbackSave, "Fallback Save", "Save settings also saves the fallback storage.");        


        spRegister(saveSettings, "Save Settings", "Save current settings to persistent storage.");
        spRegister(restoreSettings, "Restore Settings", "Restore saved settings.");
        spRegister(clearSettings, "Clear Settings", "Erase saved settings.");

        spRegister(saveFallback, "Save to Fallback", "Save system settings the the fallback storage.");                
        spRegister(restoreFallback, "Restore from Fallback", "Restore system settings from the fallback storage.");        


        spark.add(this);
    }

    bool saveObjectToStorage(spObject*, spStorage *);
    bool restoreObjectFromStorage(spObject*, spStorage *);    

    spStorage * _primaryStorage;
    spStorage * _fallbackStorage;
};
extern spSettingsSave &spSettings;
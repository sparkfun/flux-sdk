

// Action to control save of settings ...
#pragma once

#include "flxCore.h"
#include "spSpark.h"
#include "flxStorage.h"

class spSettingsSave : public flxActionType<spSettingsSave>
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
    flxPropertyBool<spSettingsSave> saveOnEvent = {true};
    flxPropertyBool<spSettingsSave> restoreOnEvent = {true};

    flxPropertyBool<spSettingsSave> fallbackSave = {false};    
    flxPropertyBool<spSettingsSave> fallbackRestore = {true};

    // Our input parameters
    flxParameterInVoid<spSettingsSave, &spSettingsSave::save_settings> saveSettings;
    flxParameterInVoid<spSettingsSave, &spSettingsSave::restore_settings> restoreSettings;
    flxParameterInVoid<spSettingsSave, &spSettingsSave::reset> clearSettings;

    flxParameterInVoid<spSettingsSave, &spSettingsSave::restore_fallback> restoreFallback;
    flxParameterInVoid<spSettingsSave, &spSettingsSave::save_fallback> saveFallback;    


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

    bool saveObjectToStorage(flxObject*, flxStorage *);
    bool restoreObjectFromStorage(flxObject*, flxStorage *);    

    flxStorage * _primaryStorage;
    flxStorage * _fallbackStorage;
};
extern spSettingsSave &spSettings;
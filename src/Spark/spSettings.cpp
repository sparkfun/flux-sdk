

#include "spSettings.h"

// Global object - for quick access to Settings system
spSettingsSave &spSettings = spSettingsSave::get();
//------------------------------------------------------------
// settings/storage things.
//------------------------------------------------------------
//
// Storage device for settings - set this in the system.

void spSettingsSave::setStorage(flxStorage *pStorage)
{
    _primaryStorage = pStorage;

}


void spSettingsSave::setFallback(flxStorage *pStorage)
{
    _fallbackStorage = pStorage;

}


//----------------------------------------------------------------------------------
// Save section
//----------------------------------------------------------------------------------

// General save routine
bool spSettingsSave::saveObjectToStorage(flxObject* pObject, flxStorage *pStorage)
{
    if (!pStorage)
        return false;
    
    // Start storage transaction
    if (!pStorage->begin())
        return false;

    bool status = pObject->save(pStorage);

    pStorage->end();

    return status;
}

//----------------------------------------------------------------------------------
// Save system

bool spSettingsSave::saveSystem(void)
{
    return save(&spark);
}
//----------------------------------------------------------------------------------
// save a specific object
bool spSettingsSave::save(flxObject &theObject)
{
    return save(&theObject);
}

//----------------------------------------------------------------------------------
// Save settings for a object
bool spSettingsSave::save(flxObject *pObject)
{
    if (!_primaryStorage)
        return false;

    bool status = saveObjectToStorage(pObject, _primaryStorage);

    if(!status)
        flxLog_E(F("Unable to save %s to %s"), pObject->name(), _primaryStorage->name());

    // Save to secondary ? 
    if (fallbackSave() && _fallbackStorage !=nullptr)
    {
        if (!saveObjectToStorage(pObject, _fallbackStorage))
            flxLog_W(F("Unable to save %s to the fallback system, %s"), pObject->name(), _fallbackStorage->name());
    }

    return status;

}

//----------------------------------------------------------------------------------
// Restore section
//----------------------------------------------------------------------------------
bool spSettingsSave::restoreObjectFromStorage(flxObject* pObject, flxStorage *pStorage)
{
    if (!pStorage)
        return false;
    
    // Start storage transaction = read-only
    if (!pStorage->begin(true))
        return false;

    bool status = pObject->restore(pStorage);

    pStorage->end();

    return status;
}

//----------------------------------------------------------------------------------
bool spSettingsSave::restoreSystem(void)
{
    return restore(&spark);
}

//----------------------------------------------------------------------------------
// restore a specific object
bool spSettingsSave::restore(flxObject &theObject)
{
    return restore(&theObject);
}

//----------------------------------------------------------------------------------
// pointer version
bool spSettingsSave::restore(flxObject *pObject)
{

    if (!_primaryStorage)
        return false;

    bool status = restoreObjectFromStorage(pObject, _primaryStorage);

    char * strSource = nullptr;
    if(!status)
    {
        flxLog_D(F("Unable to restore %s from %s"), pObject->name(), _primaryStorage->name());


        // Save to secondary ? 
        if (fallbackRestore() && _fallbackStorage !=nullptr)
        {
            status = restoreObjectFromStorage(pObject, _fallbackStorage);
            if (!status)
                flxLog_D(F("Unable to restore %s from the fallback system, %s"), pObject->name(), _fallbackStorage->name());
            else 
            {
                //flxLog_I(F("Restored settings for %s from %s"), pObject->name(), _fallbackStorage->name());
                // We restored from fallback, now save to main storage -- TODO - should this be a setting
                flxLog_D(F("Saving settings to %s"), _primaryStorage->name());
                strSource = (char*)_fallbackStorage->name();
                bool tmp = fallbackSave();
                fallbackSave=false;
                save(pObject);
                fallbackSave = tmp;
            }
        }
    }else
        strSource = (char*) _primaryStorage->name();

    if (status)
        flxLog_N(F("restored from %s"), strSource);
    else
        flxLog_N(F("unable to restore settings, using defaults"));

    return status;

}

//----------------------------------------------------------------------------------

void spSettingsSave::reset(void)
{
    if (_primaryStorage)
        _primaryStorage->resetStorage();

    if (_fallbackStorage)
        _fallbackStorage->resetStorage();

}

//----------------------------------------------------------------------------------
// Callbacks for input parameters
//----------------------------------------------------------------------------------
void spSettingsSave::restore_fallback(void)
{
    if (!_fallbackStorage)
        return;

    if (!restoreObjectFromStorage(&spark, _fallbackStorage))
        flxLog_E(F("Unable to restore settings from %s"), _fallbackStorage->name());
}
//----------------------------------------------------------------------------------
void spSettingsSave::save_fallback(void)
{
    if (!_fallbackStorage)
        return;

    if (!saveObjectToStorage(&spark, _fallbackStorage))
        flxLog_E(F("Unable to save settings to %s"), _fallbackStorage->name());
}
//------------------------------------------------------------------------------
// Events
// Slots for signals - Enables saving and restoring settings base on events
void spSettingsSave::listenForSave(flxSignalVoid &theEvent)
{
    theEvent.call(this, &spSettingsSave::saveEvent_CB);
};
void spSettingsSave::listenForRestore(flxSignalVoid &theEvent)
{
    theEvent.call(this, &spSettingsSave::restoreEvent_CB);
};


#include "spSettings.h"

// Global object - for quick access to Settings system
spSettingsSave &spSettings = spSettingsSave::get();
//------------------------------------------------------------
// settings/storage things.
//------------------------------------------------------------
//
// Storage device for settings - set this in the system.

void spSettingsSave::setStorage(spStorage &theStorage)
{
    setStorage(&theStorage);
}
void spSettingsSave::setStorage(spStorage *pStorage)
{
    _settingsStorage = pStorage;
}

// Save settings - if no parameter is passed in, the entire system is saved

bool spSettingsSave::saveSystem(void)
{
    if (!_settingsStorage)
        return false;

    spark.save(_settingsStorage);

    return true;
}
// save a specific object
bool spSettingsSave::save(spObject &theObject)
{
    return save(&theObject);
}
bool spSettingsSave::save(spObject *pObject)
{
    if (!_settingsStorage)
        return false;

    pObject->save(_settingsStorage);
    return true;
}

// Restore settings - if no parameter is passed in, the entire system is restored

bool spSettingsSave::restoreSystem(void)
{
    if (!_settingsStorage)
        return false;

    spark.restore(_settingsStorage);

    return true;
}
// restore a specific object
bool spSettingsSave::restore(spObject &theObject)
{
    return restore(&theObject);
}
bool spSettingsSave::restore(spObject *pObject)
{
    if (!_settingsStorage)
        return false;

    pObject->restore(_settingsStorage);
    return true;
}

void spSettingsSave::reset(void)
{
    if (_settingsStorage)
        _settingsStorage->resetStorage();
}

//------------------------------------------------------------------------------
// Slots for signals - Enables saving and restoring settings base on events
void spSettingsSave::listenForSave(spSignalVoid &theEvent)
{
    theEvent.call(this, &spSettingsSave::saveEvent_CB);
};
void spSettingsSave::listenForRestore(spSignalVoid &theEvent)
{
    theEvent.call(this, &spSettingsSave::restoreEvent_CB);
};
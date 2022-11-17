

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
    _vStorage.insert(_vStorage.begin(), pStorage);

}

// Save settings - if no parameter is passed in, the entire system is saved

bool spSettingsSave::saveSystem(void)
{
    if (_vStorage.size() == 0)
        return false;

    auto itStorage = _vStorage.begin();

    for(int i=0; itStorage != _vStorage.end(); itStorage++, i++)
    {
        // Start storage transaction
        if (!(*itStorage)->begin())
        {
            spLog_E(F("Unable to save settings to %s."), (*itStorage)->name());
            continue;
        }
        spark.save((*itStorage));
        (*itStorage)->end();

        // For now, we only save the main, primary storage item (TODO - revisit)
        if (i == 0)
            break;
    }

    return true;
}
// save a specific object
bool spSettingsSave::save(spObject &theObject)
{
    return save(&theObject);
}
bool spSettingsSave::save(spObject *pObject)
{
    if (_vStorage.size() == 0)
        return false;

    auto itStorage = _vStorage.begin();

    for(int i=0; itStorage != _vStorage.end(); itStorage++, i++)
    {
        // Start storage transaction
        if (!(*itStorage)->begin())
        {
            spLog_E(F("Unable to save settings to %s."), (*itStorage)->name());
            continue;
        }
        pObject->save((*itStorage));
        (*itStorage)->end();
    }

    return true;
}

// Restore settings - if no parameter is passed in, the entire system is restored

bool spSettingsSave::restoreSystem(void)
{
    if (_vStorage.size() == 0)
        return false;

    bool status = false;
    auto itStorage = _vStorage.begin();
    for(int i=0; itStorage != _vStorage.end(); itStorage++, i++)
    {
        // begin - set system in readonly mode 
        status = (*itStorage)->begin(true);

        if ( status )
        {
            status = spark.restore((*itStorage));

            (*itStorage)->end();
        }

        // success?
        if (status)
            break;
        
        spLog_D(F("Error loading settings from %s storage"), (*itStorage)->name());

        // if we are here, the read failed for the current device.

        // if this is the first iteration, and the use of secondary sources is disabled,
        // break out
        if ( i ==0 && useSecondarySources() == false)
            break;
     }   
    return status;
}
// restore a specific object
bool spSettingsSave::restore(spObject &theObject)
{
    return restore(&theObject);
}
bool spSettingsSave::restore(spObject *pObject)
{
    if (_vStorage.size() == 0)
        return false;

    bool status = false;
    auto itStorage = _vStorage.begin();
    for(int i=0; itStorage != _vStorage.end(); itStorage++, i++)
    {
        // begin - set system in readonly mode 
        status = (*itStorage)->begin(true);

        if ( status )
        {
            status = pObject->restore((*itStorage));

            (*itStorage)->end();
        }

        // success?
        if (status)
            break;
        
        spLog_D(F("Error loading settings from %s storage"), (*itStorage)->name());

        // if we are here, the read failed for the current device.

        // if this is the first iteration, and the use of secondary sources is disabled,
        // break out
        if ( i ==0 && useSecondarySources() == false)
            break;
     }   
    return status;

}

void spSettingsSave::reset(void)
{
    for (auto theStorage : _vStorage)
        theStorage->resetStorage();
}

void spSettingsSave::restore_secondary(void)
{
    if (_vStorage.size() < 2)
        return;

    spStorage *pStorage = _vStorage.at(1);

    if (!pStorage)
        return;

    bool status = pStorage->begin(true);
    if (!status)
    {
        spLog_E(F("Error starting file restore"));
        return;
    } 
    status = spark.restore(pStorage);

    pStorage->end();
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
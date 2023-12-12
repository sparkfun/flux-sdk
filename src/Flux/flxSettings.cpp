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

#include "flxSettings.h"

// Global object - for quick access to Settings system
flxSettingsSave &flxSettings = flxSettingsSave::get();
//------------------------------------------------------------
// settings/storage things.
//------------------------------------------------------------
//
// Storage device for settings - set this in the system.

void flxSettingsSave::setStorage(flxStorage *pStorage)
{
    _primaryStorage = pStorage;
}

void flxSettingsSave::setFallback(flxStorage *pStorage)
{
    _fallbackStorage = pStorage;

    // Update descr on fallback props
    char szBuffer[124];

    snprintf(szBuffer, sizeof(szBuffer), "%s (%s)", fallbackRestore.description(), pStorage->name());
    fallbackRestore.setDescription(szBuffer);

    snprintf(szBuffer, sizeof(szBuffer), "%s (%s)", fallbackSave.description(), pStorage->name());
    fallbackSave.setDescription(szBuffer);

    snprintf(szBuffer, sizeof(szBuffer), "%s (%s)", saveFallback.description(), pStorage->name());
    saveFallback.setDescription(szBuffer);

    snprintf(szBuffer, sizeof(szBuffer), "%s (%s)", restoreFallback.description(), pStorage->name());
    restoreFallback.setDescription(szBuffer);

    snprintf(szBuffer, sizeof(szBuffer), fallbackBuffer.name(), pStorage->name());
    fallbackBuffer.setName(szBuffer);
}

//----------------------------------------------------------------------------------
// Save section
//----------------------------------------------------------------------------------

// General save routine
bool flxSettingsSave::saveObjectToStorage(flxObject *pObject, flxStorage *pStorage)
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

bool flxSettingsSave::saveSystem(void)
{
    return save(&flux);
}
//----------------------------------------------------------------------------------
// save a specific object
bool flxSettingsSave::save(flxObject &theObject, bool primary_only)
{
    return save(&theObject, primary_only);
}

//----------------------------------------------------------------------------------
// Save settings for a object
bool flxSettingsSave::save(flxObject *pObject, bool primary_only)
{
    if (!_primaryStorage)
        return false;

    bool status = saveObjectToStorage(pObject, _primaryStorage);

    if (!status)
        flxLog_E(F("Unable to save %s to %s"), pObject->name(), _primaryStorage->name());

    // Save to secondary ?
    if (!primary_only && fallbackSave() && _fallbackStorage != nullptr)
    {
        if (!saveObjectToStorage(pObject, _fallbackStorage))
            flxLog_W(F("Unable to save %s to the fallback system, %s"), pObject->name(), _fallbackStorage->name());
    }

    return status;
}

//----------------------------------------------------------------------------------
// Restore section
//----------------------------------------------------------------------------------
bool flxSettingsSave::restoreObjectFromStorage(flxObject *pObject, flxStorage *pStorage)
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
bool flxSettingsSave::restoreSystem(void)
{
    return restore(&flux);
}

//----------------------------------------------------------------------------------
// restore a specific object
bool flxSettingsSave::restore(flxObject &theObject)
{
    return restore(&theObject);
}

//----------------------------------------------------------------------------------
// pointer version
bool flxSettingsSave::restore(flxObject *pObject)
{

    if (!_primaryStorage)
        return false;

    bool status = restoreObjectFromStorage(pObject, _primaryStorage);

    char *strSource = nullptr;
    if (!status)
    {
        flxLog_D(F("Unable to restore %s from %s"), pObject->name(), _primaryStorage->name());

        // Save to secondary ?
        if (fallbackRestore() && _fallbackStorage != nullptr)
        {
            status = restoreObjectFromStorage(pObject, _fallbackStorage);
            if (!status)
                flxLog_D(F("Unable to restore %s from the fallback system, %s"), pObject->name(),
                         _fallbackStorage->name());
            else
            {
                // flxLog_I(F("Restored settings for %s from %s"), pObject->name(), _fallbackStorage->name());
                //  We restored from fallback, now save to main storage -- TODO - should this be a setting
                flxLog_D(F("Saving settings to %s"), _primaryStorage->name());
                strSource = (char *)_fallbackStorage->name();
                // save the new settings - to primary storage only
                save(pObject, true);
            }
        }
    }
    else
        strSource = (char *)_primaryStorage->name();

    if (status)
        flxLog_N(F("restored from %s"), strSource);
    else
        flxLog_N(F("unable to restore settings, using defaults"));

    return status;
}

//----------------------------------------------------------------------------------

void flxSettingsSave::reset(void)
{
    if (_primaryStorage)
        _primaryStorage->resetStorage();

    if (_fallbackStorage)
        _fallbackStorage->resetStorage();
}

//----------------------------------------------------------------------------------
// Callbacks for input parameters
//----------------------------------------------------------------------------------
void flxSettingsSave::restore_fallback(void)
{
    if (!_fallbackStorage)
        return;

    if (!restoreObjectFromStorage(&flux, _fallbackStorage))
        flxLog_E(F("Unable to restore settings from %s"), _fallbackStorage->name());
    else
    {
        flxLog_I(F("\tSettings restored from %s"), _fallbackStorage->name());
        // now save these restore settings locally
        if (saveSystem())
            flxLog_I(F("\tSettings saved to %s"), _primaryStorage->name());
        else
            flxLog_W(F("\tUnable to save settings to %s"), _primaryStorage->name());
    }
}
//----------------------------------------------------------------------------------
void flxSettingsSave::save_fallback(void)
{
    if (!_fallbackStorage)
        return;

    if (!saveObjectToStorage(&flux, _fallbackStorage))
        flxLog_E(F("Unable to save settings to %s"), _fallbackStorage->name());
}
//------------------------------------------------------------------------------
// Events
// Slots for signals - Enables saving and restoring settings base on events
void flxSettingsSave::listenForSave(flxSignalVoid &theEvent)
{
    theEvent.call(this, &flxSettingsSave::saveEvent_CB);
};
void flxSettingsSave::listenForRestore(flxSignalVoid &theEvent)
{
    theEvent.call(this, &flxSettingsSave::restoreEvent_CB);
};
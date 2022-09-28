

#include "spCore.h"
#include "spStorage.h"

//////////////////////////////////////////////////////////////////////
// Object Property Interface
//////////////////////////////////////////////////////////////////////
//
/*

////////////////////////////////////////////////////////////////////////////////////////
// Define a simple class hierarchy interface definitions. Used to walk the hierarchy.
////////////////////////////////////////////////////////////////////////////////////////
//
// spBase - base class
//
// Counter to help with object naming
uint16_t spBase::_name_count = 1;

//---------------------------------------------------------------------------
// getID()
//
// Return a unique id for the class - used with serialization...
//
uint16_t spBase::getID(void)
{

    if (!_id)
    {
        if (!name.size())
        { // no name, make one
            char szBuffer[12];
            snprintf(szBuffer, sizeof(szBuffer), "spark%03d", _name_count++);
            name = szBuffer;
        }
        // hash the name - should be unique
        _id = id_hash(name);
    }
    return _id;
}

bool spBase::save(void)
{

    int id = getID(); // object id.

    size_t sz = spIProperty::save_size();

    if (sz > 0)
    {
        spStorageBlock *BLK = spStorage().beginBlock(id, sz);
        bool rc = spIProperty::save(BLK);
        if (!rc)
            Serial.println("[Error] - storage:save operation failed");

        spStorage().endBlock(BLK);
    }

    return true;
}
//-----------------------------------------------------------------
bool spBase::restore(void)
{

    // TODO: something isn't fully working here
    // Serial.print("BASE RESTORE: "); Serial.println(this->getName());
    uint16_t id = getID(); // object id.

    spStorageBlock *BLK = spStorage().beginBlock(id, 0); // 0 size = use what's in the repo

    bool rc = spIProperty::restore(BLK);

    if (!rc)
        Serial.println("[Error] - storage: restore failed.");
    spStorage().endBlock(BLK);

    return true;
}

//-----------------------------------------------------------------
bool spBase::serializeJSON(JsonObject &jObj)
{

    // Place the property values + the object name in the Json Object provided.
    jObj["id_debug"] = getID();

    for (auto prop : _myProps)
    {
        // Use a method to get the value and set in the JSON document.
        // The getValue method allows the template to define a type at compile
        // time that works with the template of the Json variable
        prop->getValue(jObj.getOrAddMember(prop->name));
    }

    return true;
}
*/
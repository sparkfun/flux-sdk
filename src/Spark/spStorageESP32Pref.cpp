/*
#
 */


#ifdef ESP32

#include "spCoreLog.h"
#include "spUtils.h"
#include "spStorageESP32Pref.h"


#define kESP32HashTagSize 16

const static char *err_init = "Settings storage not initialized.";
const static char *err_tag  = "Error creating storage tag.";
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// ESP32 preference library

// Write out a bool value
bool spStorageESP32Block::writeBool(const char *tag, bool value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putBool(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out an int8 value

bool spStorageESP32Block::writeInt8(const char *tag, int8_t value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putChar(szHash, value) > 0);


}
//------------------------------------------------------------------------
// write out a int value

bool spStorageESP32Block::writeInt32(const char *tag, int32_t value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Unsigned int8  - aka uchar

bool spStorageESP32Block::writeUInt8(const char *tag, uint8_t value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putUChar(szHash, value) > 0);
}
//------------------------------------------------------------------------
bool spStorageESP32Block::writeUInt32(const char *tag, uint32_t value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putUInt(szHash, value) > 0);
}
//------------------------------------------------------------------------
// write out a float

bool spStorageESP32Block::writeFloat(const char *tag, float value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putFloat(szHash, value) > 0);
}
//------------------------------------------------------------------------
// double

bool spStorageESP32Block::writeDouble(const char *tag, double value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putDouble(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Write out a c string
bool spStorageESP32Block::writeString(const char *tag, char *value)
{
    if ( !tag )
        return false;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return false;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return false;
    }

    return ( _prefs->putString(szHash, value) > 0);
}
//------------------------------------------------------------------------
// Read value section
//------------------------------------------------------------------------

bool spStorageESP32Block::readBool(const char *tag, bool defaultValue )
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getBool(tag, defaultValue);
}
//------------------------------------------------------------------------
int8_t spStorageESP32Block::readInt8(const char *tag, int8_t defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getChar(tag, defaultValue);
}

//------------------------------------------------------------------------
int32_t spStorageESP32Block::readInt32(const char *tag, int32_t defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getInt(tag, defaultValue);
}

//------------------------------------------------------------------------
uint8_t spStorageESP32Block::readUInt8(const char *tag, uint8_t defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getUChar(tag, defaultValue);
}

//------------------------------------------------------------------------
uint32_t spStorageESP32Block::readUInt32(const char *tag, uint32_t defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getUInt(tag, defaultValue);
}

//------------------------------------------------------------------------
float spStorageESP32Block::readFloat(const char *tag, float defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getFloat(tag, defaultValue);
}

//------------------------------------------------------------------------
double spStorageESP32Block::readDouble(const char *tag, double defaultValue)
{  
    if ( !tag )
        return defaultValue;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return defaultValue;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return defaultValue;
    }

    return _prefs->getDouble(tag, defaultValue);
}

//------------------------------------------------------------------------ 
size_t spStorageESP32Block::readString(const char *tag, char *data, size_t len)
{  
    if ( !tag )
        return 0;

    if ( !_prefs )
    {
        spLog_E(err_init);
        return 0;
    }

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {
        spLog_E(err_tag);
        return 0;
    }
    return _prefs->getString(tag, data, len);
}
  
//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

spStorageESP32Pref::spStorageESP32Pref()
{
    _theBlock.setPrefs(&_prefs);
} 

    // public methods to manage a block
spStorageESP32Block *spStorageESP32Pref::beginBlock(const char *tag)
{
    if ( !tag )
        return nullptr;

    char szHash[kESP32HashTagSize]={0};
    
    if(!sp_utils::id_hash_string_to_string(tag, szHash, sizeof(szHash)))
    {

        spLog_E(err_tag);

        return nullptr;
    }

    if ( ! _prefs.begin(szHash, false))
    {
        spLog_E("Error creating settings storage");
        return nullptr;
    }

    return &_theBlock;

}

spStorageESP32Block *spStorageESP32Pref::getBlock(const char *tag)
{

    return beginBlock(tag);
}
void spStorageESP32Pref::endBlock(spStorageBlock2 *)
{

    _prefs.end();
}

void spStorageESP32Pref::resetStorage()
{

    _prefs.clear(); // really only works on current name space ...
}


#endif
/*
#
 */

#include "spStorageJSONPref.h"
#include "spCoreLog.h"
#include "spUtils.h"


#define kJsonDocumentSize 2048
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// JSON preference library

// Write out a bool value
bool spStorageJSONBlock::writeBool(const char *tag, bool value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
// write out an int8 value

bool spStorageJSONBlock::writeInt8(const char *tag, int8_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// write out an int16 value

bool spStorageJSONBlock::writeInt16(const char *tag, int16_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// write out a int value

bool spStorageJSONBlock::writeInt32(const char *tag, int32_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
// Unsigned int8  - aka uchar

bool spStorageJSONBlock::writeUInt8(const char *tag, uint8_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// Unsigned int16  - aka ushort

bool spStorageJSONBlock::writeUInt16(const char *tag, uint16_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::writeUInt32(const char *tag, uint32_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
// write out a float

bool spStorageJSONBlock::writeFloat(const char *tag, float value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
// double

bool spStorageJSONBlock::writeDouble(const char *tag, double value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}
//------------------------------------------------------------------------
// Write out a c string
bool spStorageJSONBlock::writeString(const char *tag, const char *value)
{

    if (!_jSection.isNull() && !_readOnly)
    {
        // note - using std::string() to copy the input string. The Json library 
        // assumes the pass in string is const/static - it is not
        (_jSection)[tag] = std::string(value);
        return true;
    }
    return false;

}
//------------------------------------------------------------------------
// Read value section
//------------------------------------------------------------------------

bool spStorageJSONBlock::readBool(const char *tag, bool &value, bool defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}
//------------------------------------------------------------------------
bool spStorageJSONBlock::readInt8(const char *tag, int8_t &value, int8_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}


//------------------------------------------------------------------------
bool spStorageJSONBlock::readInt16(const char *tag, int16_t &value, int16_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readInt32(const char *tag, int32_t &value, int32_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readUInt8(const char *tag, uint8_t &value, uint8_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readUInt16(const char *tag, uint16_t &value, uint16_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readUInt32(const char *tag, uint32_t &value, uint32_t defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readFloat(const char *tag, float &value, float defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
bool spStorageJSONBlock::readDouble(const char *tag, double &value, double defaultValue)
{
    if (!_jSection.isNull())
        value = (_jSection)[tag];
    else
        value = defaultValue;

    return true;
}

//------------------------------------------------------------------------
size_t spStorageJSONBlock::readString(const char *tag, char *data, size_t len)
{


    if (_jSection.isNull())
        return 0;

    std::string value = (_jSection)[tag];

    return strlcpy(data, value.c_str(), len);


}

//------------------------------------------------------------------------------
bool spStorageJSONBlock::valueExists(const char *tag)
{
     if (!_jSection.isNull())
        return _jSection.containsKey(tag);
     else
        return false;

}
//------------------------------------------------------------------------------
// spStorage
//
// Interface for a storage system to persist state of a system

bool spStorageJSONPref::begin(bool readonly)
{

    _pDocument = new DynamicJsonDocument(kJsonDocumentSize);

    if (!_pDocument)
    {
        spLog_E(F("Unable to create JSON object for preferences."));
        return false;
    }
    _readOnly = readonly;

    return true;
}

void spStorageJSONPref::end(void)
{
    if (!_readOnly)
    {
        // TODO - write out the data
        std::string value;

        if (!serializeJsonPretty(*_pDocument, value))
            spLog_E(F("Unable to generate settings JSON string"));
        else
        {
            // TODO - output to file...
            Serial.println(value.c_str());
        }
    }
    // Clear memory
    if (_pDocument)
    {
        _pDocument->clear();
        delete _pDocument;
        _pDocument = nullptr;
    }
    _readOnly = false;
}

// public methods to manage a block
spStorageJSONBlock *spStorageJSONPref::beginBlock(const char *tag)
{

    if (!tag)
        return nullptr;

    JsonObject jObj;

    // Does the object already exists?
    jObj = (*_pDocument)[tag];
    if (jObj.isNull())
        jObj = _pDocument->createNestedObject(tag);

    _theBlock.setObject(jObj);
    _theBlock.setReadOnly(_readOnly);

    return &_theBlock;
}

spStorageJSONBlock *spStorageJSONPref::getBlock(const char *tag)
{

    // TODO - find object in JSON doc, set in block and return that

    return beginBlock(tag);
}
void spStorageJSONPref::endBlock(spStorageBlock *)
{

}

void spStorageJSONPref::resetStorage()
{
    
}
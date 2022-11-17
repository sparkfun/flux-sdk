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

    if (!_fileSystem || _filename.length() == 0)
    {
        spLog_E(F("JSON Settings unavailable - no filesystem provided"));
        return false;
    }

    _pDocument = new DynamicJsonDocument(kJsonDocumentSize);
     if (!_pDocument)
    {
        spLog_E(F("Unable to create JSON object for preferences."));
        return false;
    }
    
    if ( _fileSystem->exists(_filename.c_str()))
    {
        bool status = false;
        // read in the file, parse the json

        spFSFile theFile = _fileSystem->open(_filename.c_str(), spIFileSystem::kFileRead);

        if (theFile)
        {
            size_t nBytes = theFile.size();

            if (nBytes > 0)
            {
                char * pBuffer = new char[nBytes];

                if (pBuffer)
                {
                    size_t nRead = theFile.read((uint8_t*)pBuffer, nBytes);

                    if (nRead == nBytes)
                    {
                        if (deserializeJson(*_pDocument, (const char*)pBuffer, nBytes) == DeserializationError::Ok)
                            status = true;
                    }
                }

                delete pBuffer;
            }else
                spLog_D(F("JSON Settings Begin - Empty file"));

            theFile.close();
        }else
            spLog_D(F("JSON Settings Begin: File does not exist: %s"), _filename.c_str());

        if (status == false)
            spLog_E(F("Error reading json settings file. Ignoring"));

    }
   
    _readOnly = readonly;

    return true;
}

void spStorageJSONPref::end(void)
{
    if (!_readOnly)
    {

        // Create a json string and write to the filesystem
        std::string value;
        if (!serializeJsonPretty(*_pDocument, value))
        {
            spLog_E(F("Unable to generate settings JSON string"));
        }
        else if (_fileSystem && _filename.length() > 0)
        {
            spFSFile theFile = _fileSystem->open(_filename.c_str(), spIFileSystem::kFileWrite, true);

            if (theFile)
            {
                if (theFile.write((uint8_t*)value.c_str(), value.length()) == 0)
                    spLog_E(F("Error writing JSON settings file"));
                theFile.close();

            }else
                spLog_E(F("Error opening settings file - is file system available?"));
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

void spStorageJSONPref::setFileSystem(spIFileSystem *theFilesystem)
{
    _fileSystem = theFilesystem;
}

void spStorageJSONPref::setFilename(std::string &filename)
{
    _filename = filename;
}

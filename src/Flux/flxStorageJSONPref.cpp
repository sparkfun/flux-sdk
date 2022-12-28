/*
#
 */

#include "flxStorageJSONPref.h"
#include "flxCoreLog.h"
#include "flxUtils.h"


#define kJsonDocumentSize 3600
//------------------------------------------------------------------------------
// Use tags to ID an item and move to use data types. Model after the
// JSON preference library

// Write out a bool value
bool flxStorageJSONBlock::writeBool(const char *tag, bool value)
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

bool flxStorageJSONBlock::writeInt8(const char *tag, int8_t value)
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

bool flxStorageJSONBlock::writeInt16(const char *tag, int16_t value)
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

bool flxStorageJSONBlock::writeInt32(const char *tag, int32_t value)
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

bool flxStorageJSONBlock::writeUInt8(const char *tag, uint8_t value)
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

bool flxStorageJSONBlock::writeUInt16(const char *tag, uint16_t value)
{
    if (!_jSection.isNull() && !_readOnly)
    {
        (_jSection)[tag] = value;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::writeUInt32(const char *tag, uint32_t value)
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

bool flxStorageJSONBlock::writeFloat(const char *tag, float value)
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

bool flxStorageJSONBlock::writeDouble(const char *tag, double value)
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
bool flxStorageJSONBlock::writeString(const char *tag, const char *value)
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

bool flxStorageJSONBlock::readBool(const char *tag, bool &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}
//------------------------------------------------------------------------
bool flxStorageJSONBlock::readInt8(const char *tag, int8_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag) )
    {
        value = (_jSection)[tag];
        return true;
    }
    return false;
}


//------------------------------------------------------------------------
bool flxStorageJSONBlock::readInt16(const char *tag, int16_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readInt32(const char *tag, int32_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readUInt8(const char *tag, uint8_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readUInt16(const char *tag, uint16_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readUInt32(const char *tag, uint32_t &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readFloat(const char *tag, float &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
bool flxStorageJSONBlock::readDouble(const char *tag, double &value)
{
    if (!_jSection.isNull() && _jSection.containsKey(tag))
    {
        value = (_jSection)[tag];
        return true;
    }

    return false;
}

//------------------------------------------------------------------------
size_t flxStorageJSONBlock::readString(const char *tag, char *data, size_t len)
{


    if (_jSection.isNull() || !_jSection.containsKey(tag))
        return 0;

    std::string value = (_jSection)[tag];

    return strlcpy(data, value.c_str(), len);


}

//------------------------------------------------------------------------------
bool flxStorageJSONBlock::valueExists(const char *tag)
{
     if (!_jSection.isNull())
        return _jSection.containsKey(tag);
     else
        return false;

}
//------------------------------------------------------------------------------
// flxStorage
//
// Interface for a storage system to persist state of a system

bool flxStorageJSONPref::begin(bool readonly)
{

    if (!_fileSystem || _filename.length() == 0 || !_fileSystem->enabled())
    {
        flxLog_E(F("JSON Settings unavailable - no filesystem available."));
        return false;
    }

    _pDocument = new DynamicJsonDocument(kJsonDocumentSize);
    if (!_pDocument)
    {
        flxLog_E(F("Unable to create JSON object for preferences."));
        return false;
    }

    if ( _fileSystem->exists(_filename.c_str()))
    {
        bool status = false;
        // read in the file, parse the json

        flxFSFile theFile = _fileSystem->open(_filename.c_str(), flxIFileSystem::kFileRead);

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
                flxLog_D(F("JSON Settings Begin - Empty file"));

            theFile.close();
        }else
            flxLog_I(F("JSON Settings - the file failed to open: %s"), _filename.c_str());

        if (status == false)
            flxLog_E(F("Error reading json settings file. Ignoring"));

    }
   
    _readOnly = readonly;

    return true;
}

void flxStorageJSONPref::end(void)
{
    if (!_readOnly)
    {

        // Create a json string and write to the filesystem
        std::string value;
        if (!serializeJsonPretty(*_pDocument, value))
        {
            flxLog_E(F("Unable to generate settings JSON string"));
        }
        else if (_fileSystem && _filename.length() > 0)
        {
            flxFSFile theFile = _fileSystem->open(_filename.c_str(), flxIFileSystem::kFileWrite, true);

            if (theFile)
            {
                if (theFile.write((uint8_t*)value.c_str(), value.length()) == 0)
                    flxLog_E(F("Error writing JSON settings file"));
                theFile.close();

            }else
                flxLog_E(F("Error opening settings file - is file system available?"));
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
flxStorageJSONBlock *flxStorageJSONPref::beginBlock(const char *tag)
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

flxStorageJSONBlock *flxStorageJSONPref::getBlock(const char *tag)
{

    // TODO - find object in JSON doc, set in block and return that

    return beginBlock(tag);
}
void flxStorageJSONPref::endBlock(flxStorageBlock *)
{

}

void flxStorageJSONPref::resetStorage()
{
    
}

void flxStorageJSONPref::checkName()
{
    if (_filename.length() == 0 || !_fileSystem)
        return;

    // make a better name that includes the destination  
    char szBuffer[128];
    snprintf(szBuffer, sizeof(szBuffer), "%s on the %s", _filename.c_str(), _fileSystem->name());
    setName(szBuffer);

}
void flxStorageJSONPref::setFileSystem(flxIFileSystem *theFilesystem)
{
    _fileSystem = theFilesystem;
    checkName();
}

void flxStorageJSONPref::setFilename(std::string &filename)
{
    _filename = filename;
    checkName();
}

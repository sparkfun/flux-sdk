/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

//
// Define interfaces/base classes for output
//

#pragma once

#include "flxOutput.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#include <memory>
#include <vector>

// define a simple interface to output the actual JSON document, not
// the serialized string.
//

class flxIWriterJSON
{
  public:
    virtual void write(JsonDocument &jsonDoc) = 0;
};

template <std::size_t BUFFER_SIZE> class flxFormatJSON : public flxOutputFormat
{

  public:
    //-----------------------------------------------------------------
    flxFormatJSON()
        : _spDoc{nullptr}, _buffer_size{BUFFER_SIZE}, _isFirstRun{true}, _pSectionName{nullptr}, _maxSize{0} {};

    //-----------------------------------------------------------------
    // value methods
    void logValue(const std::string &tag, bool value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int8_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int16_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int32_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint8_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint16_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint32_t value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    void logValue(const std::string &tag, float value, uint16_t precision = 3)
    {
        // no control for precision with the JSON lib, so just pass up
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, double value, uint16_t precision = 3)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const std::string &value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const char *value)
    {
        if (!_jSection.isNull())
            (_jSection)[tag] = (char *)value;
    }

    //-----------------------------------------------------------------
    // Arrays
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, flxDataArrayBool *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayInt8 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayInt16 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayInt32 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt8 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt16 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt32 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayFloat *value, uint16_t precision = 3)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayDouble *value, uint16_t precision = 3)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayString *value)
    {
        writeOutArray(tag, value);
    }
    //-----------------------------------------------------------------
    // structure cycle

    virtual void beginObservation(const char *szTitle = nullptr)
    {
        reset(); // just in case

        if (!_spDoc)
            setBufferSize(_buffer_size);

        if (!_spDoc)
            return;

        if (szTitle)
            (*_spDoc)["title"] = szTitle;
    }

    //-----------------------------------------------------------------
    void beginSection(const char *szName)
    {
        if (_spDoc)
        {
            // _jSection = _spDoc->createNestedObject(szName);

            _jSection = (*_spDoc)[szName].to<JsonObject>();
            _pSectionName = szName;
        }
    }
    //-----------------------------------------------------------------
    void endSection()
    {
        // Note: Used to delete an empty object here (caused when all params are disabled),
        // but  the delete would cause issues at times - something with how the library
        // was deleting the object -- and maybe the memory related to the key... So now
        // not doing the delete -- and checking for valid params at the logger/higher level

        _pSectionName = nullptr;
    }
    //-----------------------------------------------------------------
    void endObservation(void)
    {
        // no op
    }

    //-----------------------------------------------------------------
    virtual void writeObservation()
    {

        if (!_spDoc)
        {
            flxLogM_E(kMsgErrInvalidState, "JSON Buffer");
            return;
        }

        // If the JSON document is NULL, just skip
        if (_spDoc->isNull())
            return;

        char szBuffer[_buffer_size + 1];
        size_t n = serializeJson(*_spDoc, szBuffer, _buffer_size);

        if (n > _maxSize)
            _maxSize = n;

        // TODO: Add Error output
        if (n > _buffer_size + 1)
        {
            flxLogM_W(kMsgErrSizeExceeded, "JSON output buffer");
            szBuffer[_buffer_size] = '\0';
        }

        // dump out mime type
        if (_isFirstRun)
        {
            outputObservation("Content-Type: application/json", flxLineTypeMime);
            _isFirstRun = false;
        }
        // Send the JSON string to output writers/destinations
        outputObservation(szBuffer);

        // if we have any output writers that want the actual json document,
        // send the document.
        for (auto aWriter : _jsonWriters)
            aWriter->write(*_spDoc);
    }

    //-----------------------------------------------------------------
    void clearObservation(void)
    {
        reset();
    }

    //-----------------------------------------------------------------
    void reset(void)
    {
        if (_spDoc)
            _spDoc->clear();
    }

    // we need to promote the add methods from our subclass - these take flxWriter() interfaces
    using flxOutputFormat::add;

    //-----------------------------------------------------------------
    // Add methods to capture the json writers
    void add(flxIWriterJSON &newWriter)
    {
        add(&newWriter);
    }
    void add(flxIWriterJSON *newWriter)
    {
        _jsonWriters.push_back(newWriter);
    }

    //-----------------------------------------------------------------
    void remove(flxIWriterJSON *oldWriter)
    {
        auto iter = std::find(_jsonWriters.begin(), _jsonWriters.end(), oldWriter);

        if (iter != _jsonWriters.end())
            _jsonWriters.erase(iter);
    }

    //-----------------------------------------------------------------
    // TODO: Rethink the buffer settings -- appears to not be needed with latest ArduinoJson version.
    void setBufferSize(size_t new_size)
    {
        // Same?
        if (new_size == _buffer_size && _spDoc)
            return;

        // clear out the smart pointer
        if (_spDoc)
            _spDoc.reset();

        _spDoc = std::make_shared<JsonDocument>();
        _buffer_size = new_size;
    }

    //-----------------------------------------------------------------
    size_t bufferSize(void)
    {
        return _buffer_size;
    }

    // bring up our sub-class remove to handle standard writers
    using flxOutputFormat::remove;

    uint32_t getMaxSizeUsed(void)
    {
        return _maxSize;
    }

  protected:
    //-----------------------------------------------------------------
    template <typename T>
    void writeOutArrayDimension(JsonArray &jsonArray, T *&pData, flxDataArrayType<T> *theArray, uint16_t currentDim)
    {

        // Write out the data?
        if (currentDim == theArray->n_dimensions() - 1)
        {
            for (int i = 0; i < theArray->dimensions()[currentDim]; i++)
                jsonArray.add(*pData++);
        }
        else
        {
            // Need to recurse
            for (int i = 0; i < theArray->dimensions()[currentDim]; i++)
            {
                // JsonArray jsonNext = jsonArray.createNestedArray();
                JsonArray jsonNext = jsonArray.add<JsonArray>();
                // recurse
                writeOutArrayDimension(jsonNext, pData, theArray, currentDim + 1);
            }
        }
    }
    //-----------------------------------------------------------------
    template <typename T> void writeOutArray(const std::string &tag, flxDataArrayType<T> *theArray)
    {
        // create an array in this section

        // JsonArray jsonArray = _jSection.createNestedArray(tag);
        JsonArray jsonArray = _jSection[tag].to<JsonArray>();

        T *pData = theArray->get();

        if (!pData)
            return;
        else
            writeOutArrayDimension(jsonArray, pData, theArray, 0);
    }

    JsonObject _jSection;

    size_t _buffer_size;

    // Shared pointer to the JSON document
    std::shared_ptr<JsonDocument> _spDoc;

  private:
    std::vector<flxIWriterJSON *> _jsonWriters;

    bool _isFirstRun;
    const char *_pSectionName;

    uint32_t _maxSize;
};

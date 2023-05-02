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
 
//
// Define interfaces/base classes for output
//

#pragma once

#include "flxOutput.h"

#include <Arduino.h>
#include <ArduinoJson.h>

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
    flxFormatJSON() : buffer_size{BUFFER_SIZE}, _isFirstRun{true} {};

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
    void logValue(const std::string &tag, int value)
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
    void logValue(const std::string &tag, uint value)
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
            (_jSection)[tag] = (char*)value;
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
    void logValue(const std::string &tag, flxDataArrayInt *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUint8 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUint16 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, flxDataArrayUint *value)
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
        reset(); // just incase
        if (szTitle)
            _jDoc["title"] = szTitle;
    }

    //-----------------------------------------------------------------
    void beginSection(const char *szName)
    {
        _jSection = _jDoc.createNestedObject(szName);
    }
    //-----------------------------------------------------------------
    void endObservation(void)
    {
        // no op
    }

    //-----------------------------------------------------------------
    virtual void writeObservation()
    {
        char szBuffer[buffer_size + 1];
        size_t n = serializeJson(_jDoc, szBuffer, buffer_size);

        // TODO: Add Error output
        if (n > buffer_size + 1)
        {
            flxLog_W(" JSON document buffer output buffer trimmed");
            szBuffer[buffer_size] = '\0';
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
        // send the mthe document.

        for ( auto aWriter : _jsonWriters)
            aWriter->write(_jDoc);
    }

    //-----------------------------------------------------------------
    void clearObservation(void)
    {
        reset();
    }

    //-----------------------------------------------------------------
    void reset(void)
    {
        _jDoc.clear();
    }

    size_t buffer_size;

    // we need to promote the add methods from our subclass - these take flxWriter() interfaces
    using flxOutputFormat::add;

    // Add methods to capture the json writers
    void add(flxIWriterJSON &newWriter)
    {
        add(&newWriter);
    }
    void add(flxIWriterJSON *newWriter)
    {
        _jsonWriters.push_back(newWriter);
    }
    void remove(flxIWriterJSON *oldWriter)
    {
        auto iter = std::find(_jsonWriters.begin(), _jsonWriters.end(), oldWriter);

        if (iter != _jsonWriters.end())
            _jsonWriters.erase(iter);
    }

    // bring up our sub-class remove to handle standard writers
    using flxOutputFormat::remove;


  protected:
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
                JsonArray jsonNext = jsonArray.createNestedArray();
                // recurse
                writeOutArrayDimension(jsonNext, pData, theArray, currentDim + 1);
            }
        }
    }
    //-----------------------------------------------------------------
    template <typename T> void writeOutArray(const std::string &tag, flxDataArrayType<T> *theArray)
    {
        // create an array in this section

        JsonArray jsonArray = _jSection.createNestedArray(tag);

        T *pData = theArray->get();

        if (!pData)
            return;
        else
            writeOutArrayDimension(jsonArray, pData, theArray, 0);
    }

    JsonObject _jSection;

    StaticJsonDocument<BUFFER_SIZE> _jDoc;

private:
    std::vector<flxIWriterJSON *>  _jsonWriters;

    bool _isFirstRun;
};

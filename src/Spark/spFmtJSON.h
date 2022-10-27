//
// Define interfaces/base classes for output
//

#pragma once

#include "spOutput.h"

#include <Arduino.h>
#include <ArduinoJson.h>

template <std::size_t BUFFER_SIZE> class spFormatJSON : public spOutputFormat
{

  public:
    //-----------------------------------------------------------------
    spFormatJSON() : buffer_size{BUFFER_SIZE} {};

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
            (_jSection)[tag] = value;
    }

    //-----------------------------------------------------------------
    // Arrays
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, spDataArrayBool *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayInt8 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayInt16 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayInt *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayUint8 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayUint16 *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayUint *value)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayFloat *value, uint16_t precision = 3)
    {
        writeOutArray(tag, value);
    }
    void logValue(const std::string &tag, spDataArrayDouble *value, uint16_t precision = 3)
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
            spLog_W(" JSON document buffer output buffer trimmed");
            szBuffer[buffer_size] = '\0';
        }

        outputObservation(szBuffer);
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

  protected:
    template <typename T>
    void writeOutArrayDimension(JsonArray &jsonArray, T *&pData, spDataArrayType<T> *theArray, uint16_t currentDim)
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
    template <typename T> void writeOutArray(const std::string &tag, spDataArrayType<T> *theArray)
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
};

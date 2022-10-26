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
    
    void logValue(const std::string &tag, float value, uint16_t precision=3)
    {
        // no control for precision with the JSON lib, so just pass up
        if (!_jSection.isNull())
            (_jSection)[tag] = value;
    }
    
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, double value, uint16_t precision=3)
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

    }
    void logValue(const std::string &tag, spDataArrayInt8 *value)
    {

    }
    void logValue(const std::string &tag, spDataArrayInt16 *value)
    {

    }
    void logValue(const std::string &tag, spDataArrayInt *value )
    {

    }   
    void logValue(const std::string &tag, spDataArrayUint8 *value)
    {

    }
    void logValue(const std::string &tag, spDataArrayUint16 *value)
    {

    }
    void logValue(const std::string &tag, spDataArrayUint *value)
    {

    }
    void logValue(const std::string &tag, spDataArrayFloat *value, uint16_t precision=3)
    {

    }
    void logValue(const std::string &tag, spDataArrayDouble *value, uint16_t precision=3)
    {
        
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
    JsonObject _jSection;

    StaticJsonDocument<BUFFER_SIZE> _jDoc;
};

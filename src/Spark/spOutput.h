//
// Define interfaces/base classes for output
//

#pragma once
#include <string>
#include <vector>

#include "spCoreInterface.h"
#include "spCoreTypes.h"
//-----------------------------------------

// Define a formatter for log data

class spOutputFormat
{

  public:
    spOutputFormat(){};

    // value methods
    virtual void logValue(const std::string &tag, bool value) = 0;
    virtual void logValue(const std::string &tag, int8_t value) = 0;
    virtual void logValue(const std::string &tag, int16_t value) = 0;
    virtual void logValue(const std::string &tag, int value) = 0;
    virtual void logValue(const std::string &tag, uint8_t value) = 0;
    virtual void logValue(const std::string &tag, uint16_t value) = 0;
    virtual void logValue(const std::string &tag, uint value) = 0;
    virtual void logValue(const std::string &tag, float value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, double value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, const char *value) = 0;
    virtual void logValue(const std::string &tag, std::string &value)
    {
        logValue(tag, value.c_str());
    }

    // Array value methods
    virtual void logValue(const std::string &tag, spDataArrayBool *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayInt8 *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayInt16 *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayInt *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayUint8 *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayUint16 *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayUint *value) = 0;
    virtual void logValue(const std::string &tag, spDataArrayFloat *value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, spDataArrayDouble *value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, spDataArrayString *value) = 0;

    // structure cycle

    virtual void beginObservation(const char *szTitle = nullptr) = 0;
    virtual void beginSection(const char *szName){};
    virtual void beginSection(const std::string &name)
    {
        beginSection(name.c_str());
    }
    virtual void endSection(void){};
    virtual void endObservation(void) = 0;

    virtual void writeObservation(void) = 0;
    virtual void clearObservation(void){};

    virtual void reset(void){};

    void add(spWriter &newWriter)
    {
        add(&newWriter);
    }
    void add(spWriter *newWriter)
    {
        _Writers.push_back(newWriter);
    }

    void remove(spWriter *oldWriter)
    {
        auto iter = std::find(_Writers.begin(), _Writers.end(), oldWriter);

        if (iter != _Writers.end())
            _Writers.erase(iter);
    }

    void outputObservation(const char *szBuffer)
    {

        for (auto writer : _Writers)
            writer->write(szBuffer);
    }

  private:
    std::vector<spWriter *> _Writers;
};

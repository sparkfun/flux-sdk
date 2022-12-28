//
// Define interfaces/base classes for output
//

#pragma once
#include <string>
#include <vector>

#include "spCoreInterface.h"
#include "flxCoreTypes.h"
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
    virtual void logValue(const std::string &tag, flxDataArrayBool *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayInt8 *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayInt16 *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayInt *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayUint8 *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayUint16 *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayUint *value) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayFloat *value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayDouble *value, uint16_t precision = 3) = 0;
    virtual void logValue(const std::string &tag, flxDataArrayString *value) = 0;

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

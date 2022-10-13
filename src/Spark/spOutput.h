//
// Define interfaces/base classes for output
//

#pragma once
#include <string>
#include <vector>
//-----------------------------------------
// Define an interface for output of log information.

class spWriter
{

  public:
    virtual void write(bool value)
    {
        write((int)value);
    };
    virtual void write(int) = 0;
    virtual void write(float) = 0;
    virtual void write(const char *) = 0;
    virtual void write(std::string &value)
    {
        write(value.c_str());
    };
};

// Define a formatter for log data

class spOutputFormat
{

  public:
    spOutputFormat(){};

    // value methods
    virtual void logValue(const std::string &tag, bool value) = 0;
    virtual void logValue(const std::string &tag, int value) = 0;
    virtual void logValue(const std::string &tag, uint value) = 0;
    virtual void logValue(const std::string &tag, float value, uint16_t precision=3) = 0;
    virtual void logValue(const std::string &tag, double value, uint16_t precision=3) = 0;
    virtual void logValue(const std::string &tag, const char *value) = 0;
    virtual void logValue(const std::string &tag, std::string &value)
    {
        logValue(tag, value.c_str());
    }

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

    void outputObservation(const char *szBuffer)
    {

        for (auto writer : _Writers)
            writer->write(szBuffer);
    }

  private:
    std::vector<spWriter *> _Writers;
};



#pragma once

// Core interfaces for the system....

#include <string>
//----------------------------------------------------------------------
// spDataEditor()
//
// define a data editor interface - called to have some value edited.
//
//
// TODO: Factor to a different header in the future...

class spDataEditor
{

  public:
    virtual bool editField(char *value, size_t lenValue, uint32_t timeout = 60) = 0;
    virtual bool editField(std::string &value, uint32_t timeout = 60) = 0;
    virtual bool editField(bool &value, uint32_t timeout = 60) = 0;
    virtual bool editField(int8_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(int16_t &value, uint32_t timeout = 60) = 0;    
    virtual bool editField(int32_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(uint8_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(uint16_t &value, uint32_t timeout = 60) = 0;    
    virtual bool editField(uint32_t &value, uint32_t timeout = 60) = 0;
    virtual bool editField(float &value, uint32_t timeout = 60) = 0;
    virtual bool editField(double &value, uint32_t timeout = 60) = 0;
    virtual void beep() = 0;
};


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

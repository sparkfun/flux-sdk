/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

// Core interfaces for the system....

#include <string>

//----------------------------------------------------------------------
// flxDataEditor()
//
// define a data editor interface - called to have some value edited.
//
//
// TODO: Factor to a different header in the future...

class flxDataEditor
{

  public:
    virtual bool editField(char *value, size_t lenValue, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(std::string &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(bool &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(int8_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(int16_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(int32_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(uint8_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(uint16_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(uint32_t &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(float &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual bool editField(double &value, bool hidden = false, uint32_t timeout = 60) = 0;
    virtual void beep() = 0;
};

// Define an interface for output of log information.
typedef enum
{
    flxLineTypeNone = 0,
    flxLineTypeData = 1,
    flxLineTypeHeader = 2,
    flxLineTypeMime = 4
} flxLineType_t;

class flxWriter
{

  public:
    virtual void write(bool value)
    {
        write((int32_t)value);
    };
    virtual void write(int32_t) = 0;
    virtual void write(float) = 0;
    virtual void write(const char *value, bool newline, flxLineType_t type) = 0;
    virtual void write(const char *value, bool newline)
    {
        write(value, newline, flxLineTypeData);
    };
    virtual void write(std::string &value, bool newline)
    {
        write(value.c_str(), newline);
    }
    virtual void write(const char *value)
    {
        write(value, true);
    }
    virtual void write(std::string &value)
    {
        write(value.c_str(), true);
    };
    // Color testing
    virtual bool colorEnabled(void)
    {
        return false;
    }
    virtual void textToRed(void) {};
    virtual void textToGreen(void) {};
    virtual void textToYellow(void) {};
    virtual void textToBlue(void) {};
    virtual void textToWhite(void) {};
    virtual void textToNormal(void) {};
    virtual void textToCyan(void) {};
    virtual void textToMagenta(void) {};
};

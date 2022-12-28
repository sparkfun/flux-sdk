

#pragma once

#include <ctype.h>
#include <string>

#include "flxCore.h"
#include "flxCoreTypes.h"

class spSerialField : public flxDataEditor
{

  public:
    bool editField(char *value, size_t lenValue, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldCString(value, lenValue, hidden, timeout);
    }

    bool editField(bool &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldBool(value, hidden, timeout);
    }

    bool editField(std::string &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldString(value, hidden, timeout);
    }
    bool editField(int8_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldInt8(value, hidden, timeout);
    }
    bool editField(int16_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldInt16(value, hidden, timeout);
    }

    bool editField(int32_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldInt(value, hidden, timeout);
    }

    bool editField(uint8_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldUInt8(value, hidden, timeout);
    }

    bool editField(uint16_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldUInt16(value, hidden, timeout);
    }

    bool editField(uint32_t &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldUInt(value, hidden, timeout);
    }

    bool editField(float &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldFloat(value, hidden, timeout);
    }

    bool editField(double &value, bool hidden=false, uint32_t timeout = 60)
    {
        return editFieldDouble(value, hidden, timeout);
    }

    bool editFieldBool(bool &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldInt8(int8_t &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldInt16(int16_t &value, bool hidden=false, uint32_t timeout = 60);    
    bool editFieldInt(int32_t &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldUInt8(uint8_t &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldUInt16(uint16_t &value, bool hidden=false, uint32_t timeout = 60);    
    bool editFieldUInt(uint32_t &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldFloat(float &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldDouble(double &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldString(std::string &value, bool hidden=false, uint32_t timeout = 60);
    bool editFieldCString(char *value, size_t lenValue, bool hidden=false, uint32_t timeout = 60);
    void beep();
    
  private:
    static constexpr uint16_t kEditBufferMax = 256;
    typedef struct
    {
        char head[kEditBufferMax]; //
        char tail[kEditBufferMax]; // text after the cursor -  values at:  bcursor to kEditBufferMax-1 ;
        char all[kEditBufferMax];  // internal use
        uint cursor;
        uint bcursor;
        bool (*validator)(char *value);
        bool hidden;
    } FieldContext_t;

    void drawTrailing(FieldContext_t &ctxEdit, bool isDelete = false);
    void resetContext(FieldContext_t &ctxEdit);
    void processArrowKeys(FieldContext_t &ctxEdit, char inCode);
    void processKillToEOL(FieldContext_t &ctxEdit);
    void processDELKey(FieldContext_t &ctxEdit);
    void processBackspaceKey(FieldContext_t &ctxEdit);
    void processEndOfLineKey(FieldContext_t &ctxEdit);
    void processStartOfLineKey(FieldContext_t &ctxEdit);
    void processText(FieldContext_t &ctxEdit, char *inputBuffer, uint length);
    bool editLoop(FieldContext_t &ctxEdit, uint32_t timeout = 10);
    void fulltext(FieldContext_t &ctxEdit, char *buffer, size_t length = kEditBufferMax);


};
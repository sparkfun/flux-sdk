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

#include "flxCore.h"
#include "flxOutput.h"
#include "flxUtils.h"

#include <Arduino.h>
#include <string>

//
// A CSV output formatter

//-------------------------------------------------------------------------------------

#define kMaxCVSHeaderTagSize 32

class flxFormatCSV : public flxOutputFormat
{

  private:
    void writeHeaderEntry(const std::string &tag)
    {
        if ((_writeHeader & kHeaderWrite) == kHeaderWrite)
            if (!append_to_header(tag))
                flxLogM_W(kMsgErrSizeExceeded, "CVS header");
    }

  public:
    //-----------------------------------------------------------------
    flxFormatCSV()
    {
        reset();
    };

    //-----------------------------------------------------------------
    // value methods
    void logValue(const std::string &tag, bool value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int32_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int8_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int16_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint32_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint8_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint16_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = flx_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, float value, uint16_t precision)
    {
        logValue(tag, (double)value, precision);
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, double value, uint16_t precision)
    {
        // header?
        writeHeaderEntry(tag);

        char szBuffer[32] = {'\0'};
        (void)flx_utils::dtostr(value, szBuffer, sizeof(szBuffer), precision);

        if (!append_csv_value(szBuffer, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const std::string &value)
    {
        // header?
        writeHeaderEntry(tag);

        if (!append_csv_value(value, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const char *value)
    {
        // header?
        writeHeaderEntry(tag);

        if (!append_csv_value(std::string(value), _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    void logValue(const std::string &tag, flxDataArrayBool *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayInt8 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayInt16 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }

    void logValue(const std::string &tag, flxDataArrayInt32 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt8 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt16 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayUInt32 *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, flxDataArrayFloat *value, uint16_t precision = 3)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value, precision);
    }
    void logValue(const std::string &tag, flxDataArrayDouble *value, uint16_t precision = 3)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value, precision);
    }

    void logValue(const std::string &tag, flxDataArrayString *value)
    {
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    //-----------------------------------------------------------
    // structure cycle

    virtual void beginObservation(const char *szTitle = nullptr)
    {
        _inObservation = true;
    }

    //-----------------------------------------------------------------
    void beginSection(const char *szName)
    {
        _section_name = (char *)szName;
    }

    //-----------------------------------------------------------------
    void endSection(const char *szName)
    {
        _section_name = nullptr;
    }
    //-----------------------------------------------------------------
    void endObservation(void)
    {
        // ends the data collection - no op here
    }

    //-----------------------------------------------------------------
    virtual void writeObservation()
    {
        // Is the buffer empty?
        if (_data_buffer.length() == 0)
            return;

        // First run? output mime type
        if (_isFirstRun)
        {
            outputObservation("Content-Type: text/csv", flxLineTypeMime);
            _isFirstRun = false;
        }
        // Write out the header?
        if ((_writeHeader & kHeaderWrite) == kHeaderWrite && _header_buffer.length() > 0)
            outputObservation(_header_buffer.c_str(), flxLineTypeHeader);

        outputObservation(_data_buffer.c_str());
    }

    //-----------------------------------------------------------------
    void clearObservation(void)
    {
        clear_buffers();
        _section_name = nullptr;
        _writeHeader = (_writeHeader & kHeaderPending) == kHeaderPending ? kHeaderWrite : kHeaderNone;

        // This ends the Observation transaction...
        _inObservation = false;
    }

    //-----------------------------------------------------------------
    void reset(void)
    {
        clear_buffers();
        _writeHeader = kHeaderWrite;
        _section_name = nullptr;
        _inObservation = false;
        _isFirstRun = true;
    }

    //-----------------------------------------------------------------
    // Call this method to force the header to print next iteration.
    //
    // Normally the header is printing only for the first run.

    void output_header(void)
    {
        // For output to a file, this method is called by the new file event
        // and is often triggered in the middle of a observation transaction.
        // Given this, the writer header flag can't be set, since it's cleared at
        // the end of the observation. So, if in an observation, set the pending bit

        if (_inObservation)
        {
            // Are we writing a header with this observation? If so we are
            // starting a new file on a new system and writing data and a header
            // so skip adding a pending header
            if ((_writeHeader & kHeaderWrite) != kHeaderWrite)
                _writeHeader = (flxFmtCSVHeader_t)(_writeHeader | kHeaderPending);
        }
        else
            _writeHeader = kHeaderWrite;
    }

  private:
    //-----------------------------------------------------------------
    void clear_buffers()
    {

        // Note: clear() empties the string, but retains memory.
        //       shrink_to_fit() - frees memory
        //
        // Since the header is normally printed very rarely,
        // we shrink it.
        //
        // For data buffer, it's since is consistent between calls,
        // so just clear it.

        _header_buffer.clear();
        _header_buffer.shrink_to_fit();
        _data_buffer.clear();
    }

    bool append_csv_value(const std::string &value, std::string &buffer)
    {

        if (buffer.length() > 0)
            buffer += ',';

        buffer += value;

        return true;
    }
    //-----------------------------------------------------------------
    bool append_to_header(const std::string &tag)
    {
        if (tag.length() == 0)
            return true;

        // build up our header title for this element. Title is {section name}.{tag}
        char szBuffer[kMaxCVSHeaderTagSize] = {'\0'};

        if (_section_name)
        {
            strlcpy(szBuffer, _section_name, sizeof(szBuffer));
            strlcat(szBuffer, ".", sizeof(szBuffer));
        }
        strlcat(szBuffer, tag.c_str(), sizeof(szBuffer));

        return append_csv_value(szBuffer, _header_buffer);
    }

    //-----------------------------------------------------------------
    // Array support
    //-----------------------------------------------------------------

    template <typename T> void formatArrayValue(std::string &sData, T value, uint16_t precision = 3)
    {
        sData += flx_utils::to_string(value);
    }
    void formatArrayValue(std::string &sData, float value, uint16_t precision)
    {
        sData += flx_utils::to_string(value, precision);
    }
    void formatArrayValue(std::string &sData, double value, uint16_t precision)
    {
        sData += flx_utils::to_string(value, precision);
    }

    template <typename T>
    void writeOutArrayDimension(std::string &sData, T *&pData, flxDataArrayType<T> *theArray, uint16_t currentDim,
                                uint16_t precision = 3)
    {
        sData += "[";

        // Write out the data?
        if (currentDim == theArray->n_dimensions() - 1)
        {
            bool isFloat = (theArray->type() == flxTypeFloat || theArray->type() == flxTypeDouble);

            for (int i = 0; i < theArray->dimensions()[currentDim]; i++)
            {
                if (i > 0)
                    sData += ",";

                if (isFloat)
                    formatArrayValue(sData, *pData++, precision);
                else
                    formatArrayValue(sData, *pData++);
            }
        }
        else
        {
            // Need to recurse
            for (int i = 0; i < theArray->dimensions()[currentDim]; i++)
            {
                if (i > 0)
                    sData += ",";
                // recurse
                writeOutArrayDimension(sData, pData, theArray, currentDim + 1, precision);
            }
        }
        sData += "]";
    }
    //-----------------------------------------------------------------
    template <typename T> void writeOutArray(flxDataArrayType<T> *theArray, uint16_t precision = 3)
    {
        std::string sData = "";

        T *pData = theArray->get();

        if (!pData)
            sData = "[]";
        else
            writeOutArrayDimension(sData, pData, theArray, 0, precision);

        if (!append_csv_value(sData, _data_buffer))
            flxLogM_E(kMsgErrSizeExceeded, "CVS buffer");
    }

    //-----------------------------------------------------------------

    std::string _header_buffer;
    std::string _data_buffer;

    char *_section_name;

    // Header status field values
    typedef enum
    {
        kHeaderNone = 0,
        kHeaderWrite = 1,
        kHeaderPending = 2,
        kHeaderWriteAndPending = 3
    } flxFmtCSVHeader_t;

    flxFmtCSVHeader_t _writeHeader;

    bool _inObservation;
    bool _isFirstRun;
};

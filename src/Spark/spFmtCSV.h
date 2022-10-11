//
// Define interfaces/base classes for output
//

#pragma once

#include "spCore.h"
#include "spOutput.h"
#include "spUtils.h"

#include <Arduino.h>
#include <string>

//
// A CSV output formatter

//-------------------------------------------------------------------------------------

#define kMaxCVSHeaderTagSize 32

class spFormatCSV : public spOutputFormat
{

  public:
    //-----------------------------------------------------------------
    spFormatCSV()
    {
        reset();
    };

    //-----------------------------------------------------------------
    // value methods
    void logValue(const std::string &tag, bool value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        if (!append_csv_value(value ? "true" : "false", _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%d", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

	//-----------------------------------------------------------------
    void logValue(const std::string &tag, int8_t value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%d", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int16_t value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%d", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%u", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint8_t value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%u", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint16_t value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%u", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, float value)
    {
        logValue(tag, (double)value);
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, double value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        (void)sp_utils::dtostr(value, szBuffer, sizeof(szBuffer));

        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const std::string &value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        if (!append_csv_value(value, _header_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const char *value)
    {
        // header?
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");

        if (!append_csv_value(std::string(value), _header_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }
    //-----------------------------------------------------------------
    // structure cycle

    virtual void beginObservation(const char *szTitle = nullptr)
    {

        // if(szTitle) // TODO TITLE?
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
    }

    //-----------------------------------------------------------------
    virtual void writeObservation()
    {

        // Write out the header?
        if (_bWriteHeader && _header_buffer.length() > 0)
            outputObservation(_header_buffer.c_str());

        outputObservation(_data_buffer.c_str());
    }

    //-----------------------------------------------------------------
    void clearObservation(void)
    {
        clear_buffers();
        _section_name = nullptr;
        if (_bWriteHeader)
            _bWriteHeader = false;
    }

    //-----------------------------------------------------------------
    void reset(void)
    {
        clear_buffers();
        _bWriteHeader = true;
        _section_name = nullptr;
    }

    //-----------------------------------------------------------------
    // Call this method to force the header to print next iteration.
    //
    // Normally the header is printing only for the first run.

    void output_header(void)
    {
        _bWriteHeader = true;
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
        // For data buffer, it's since is consistant between calls,
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

    std::string _header_buffer;
    std::string _data_buffer;

    char *_section_name;

    bool _bWriteHeader;
};

//
// Define interfaces/base classes for output
//

#pragma once

#include "spCore.h"
#include "spOutput.h"
#include "spUtils.h"

#include <Arduino.h>
#include <string>

//-------------------------------------------------------------------------------------
// Use a stack based string allocator ... it basically replaces the allocator
// used with std::string with one that uses a stack memory area, and if that
// size is exceeded, uses a heap allocation scheme.
//
// See spUtil.h for more info.
#define kCSVStackMemoryBufferSize 500

template <class T, std::size_t buffSize = kCSVStackMemoryBufferSize>
using stackString = std::basic_string<T, std::char_traits<T>, short_alloc<T, buffSize, alignof(T)>>;

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
    void logValue(const char *tag, bool value)
    {
        // header?
        if (_bFirstRun)
            if (!append_to_header(tag))
                warning_message("CSV - internal header buffer size exceeded.");

        if (!append_csv_value(value ? "true" : "false", _data_buffer))
            error_message("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const char *tag, int value)
    {
        // header?
        if (_bFirstRun)
            if (!append_to_header(tag))
                warning_message("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        snprintf(szBuffer, sizeof(szBuffer), "%d", value);
        if (!append_csv_value(szBuffer, _data_buffer))
            error_message("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const char *tag, float value)
    {
        // header?
        if (_bFirstRun)
            if (!append_to_header(tag))
                warning_message("CSV - internal header buffer size exceeded.");

        char szBuffer[32] = {'\0'};
        (void)sp_utils::dtostr(value, szBuffer, sizeof(szBuffer));

        if (!append_csv_value(szBuffer, _data_buffer))
            error_message("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const char *tag, const char *value)
    {
        // header?
        if (_bFirstRun)
            if (!append_to_header(tag))
                warning_message("CSV - internal header buffer size exceeded.");

        if (!append_csv_value(value, _header_buffer))
            error_message("CSV - internal data buffer size exceeded.");
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
        if (_bFirstRun && _header_buffer.length() > 0)
            outputObservation(_header_buffer.c_str());

        outputObservation(_data_buffer.c_str());
    }

    //-----------------------------------------------------------------
    void clearObservation(void)
    {
        clear_buffers();
        _section_name = nullptr;
        if (_bFirstRun)
            _bFirstRun = false;
    }

    //-----------------------------------------------------------------
    void reset(void)
    {
        clear_buffers();
        _bFirstRun = true;
        _section_name = nullptr;
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
        // For the data_buffer, shrink it only after the first run.
        // Assumption here is:
        //	- The header is large, so will take up most of our static buffer
        //		- causes the data buffer to be allocated on the heap
        //	- The header is only printed/retained with the first run, leaving
        //    more static memory available to the data_buffer on non-first runs
        //  - Calling shrink on the data_buffer on the first run free's any
        //    heap memory, with the idea that later runs use stack memory

        _header_buffer.clear();
        _header_buffer.shrink_to_fit();
        _data_buffer.clear();

        if (_bFirstRun)
            _data_buffer.shrink_to_fit();
    }

    bool append_csv_value(const char *value, stackString<char> &buffer)
    {

        if (buffer.length() > 0)
            buffer += ',';

        buffer += value;

        return true;
    }
    //-----------------------------------------------------------------
    bool append_to_header(const char *tag)
    {
        if (!tag)
            return true;

        // build up our header title for this element. Title is {section name}.{tag}
        char szBuffer[kMaxCVSHeaderTagSize] = {'\0'};

        if (_section_name)
        {
            strlcpy(szBuffer, _section_name, sizeof(szBuffer));
            strlcat(szBuffer, ".", sizeof(szBuffer));
        }
        strlcat(szBuffer, tag, sizeof(szBuffer));

        return append_csv_value(szBuffer, _header_buffer);
    }

    //-----------------------------------------------------------------
    // Call this method to force the header to print next iteration.
    //
    // Normally the header is printing only for the first run.

    void output_header(void)
    {
        _bFirstRun = true;
    }

    //-----------------------------------------------------------------

    // For the stack based allocator  - First you create a area on the stack
    stackString<char>::allocator_type::arena_type static_allocator;

    // Create strings that use this stack areas
    stackString<char> _header_buffer{static_allocator};
    stackString<char> _data_buffer{static_allocator};

    char *_section_name;

    bool _bFirstRun;
};

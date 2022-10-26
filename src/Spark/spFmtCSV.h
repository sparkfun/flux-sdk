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

  private:
    void writeHeaderEntry(const std::string &tag)
    {
        if (_bWriteHeader)
            if (!append_to_header(tag))
                spLog_W("CSV - internal header buffer size exceeded.");
    }

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
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

	//-----------------------------------------------------------------
    void logValue(const std::string &tag, int8_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, int16_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint8_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }

    //-----------------------------------------------------------------
    void logValue(const std::string &tag, uint16_t value)
    {
        // header?
        writeHeaderEntry(tag);

        std::string stmp = sp_utils::to_string(value);
        if (!append_csv_value(stmp, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
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
        (void)sp_utils::dtostr(value, szBuffer, sizeof(szBuffer), precision);

        if (!append_csv_value(szBuffer, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const std::string &value)
    {
        // header?
        writeHeaderEntry(tag);

        if (!append_csv_value(value, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }
    //-----------------------------------------------------------------
    void logValue(const std::string &tag, const char *value)
    {
        // header?
        writeHeaderEntry(tag);
        
        if (!append_csv_value(std::string(value), _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");
    }




    void logValue(const std::string &tag, spDataArrayBool *value)
    {
        // header
        writeHeaderEntry(tag);
        writeOutArray(value);
    }
    void logValue(const std::string &tag, spDataArrayInt8 *value)
    {
        // header
        writeHeaderEntry(tag);
        
    }
    void logValue(const std::string &tag, spDataArrayInt16 *value)
    {
        // header
        writeHeaderEntry(tag);
    }
        
    void logValue(const std::string &tag, spDataArrayInt *value )
    {
        // header
        writeHeaderEntry(tag);
        
    }   
    void logValue(const std::string &tag, spDataArrayUint8 *value)
    {
        // header
        writeHeaderEntry(tag);
        
    }
    void logValue(const std::string &tag, spDataArrayUint16 *value)
    {
        // header
        writeHeaderEntry(tag);
        
    }
    void logValue(const std::string &tag, spDataArrayUint *value)
    {
        // header
        writeHeaderEntry(tag);
    
    }
    void logValue(const std::string &tag, spDataArrayFloat *value, uint16_t precision=3)
    {
        // header
        writeHeaderEntry(tag);
        
    }
    void logValue(const std::string &tag, spDataArrayDouble *value, uint16_t precision=3)
    {
        // header
        writeHeaderEntry(tag);
        
    }
    //-----------------------------------------------------------    
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
    // Array support
    //-----------------------------------------------------------------

    // 
    template <typename T>
    void writeOutArrayDimension(std::string &sData, T * &pData, uint16_t nDim, uint16_t *dims, uint16_t currentDim)
    {
        sData += "[";
        // Write out the data?
        if ( currentDim == nDim -1  )
        {
            for (int i=0; i < dims[currentDim]; i++)
            {
                if (i > 0)
                    sData += ", ";
                sData += sp_utils::to_string(*pData++);
            }
        }
        else
        {
            // Need to recurse 
            for ( int i=0; i < dims[currentDim]-1; i++)
            {
                if (i > 0)
                    sData += ", ";
                // recurse
                writeOutArrayDimension(sData, pData, nDim, dims, currentDim+1);
            }
        }
        sData += "]";
    }

    template <typename T>
    void writeOutArray(spDataArrayType<T> *theArray)
    {
        std::string sData = "";

        T * pData = theArray->get();

        if (!pData)
            sData = "[]";
        else
            writeOutArrayDimension(sData, pData,  theArray->n_dimensions(), theArray->dimensions(), 0);

        if (!append_csv_value(sData, _data_buffer))
            spLog_E("CSV - internal data buffer size exceeded.");

    }

    //-----------------------------------------------------------------

    std::string _header_buffer;
    std::string _data_buffer;

    char *_section_name;

    bool _bWriteHeader;
};

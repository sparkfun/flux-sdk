

#include "spFileRotate.h"
#include "spUtils.h"
#include "spClock.h"

// number of writes between flushes
#define kFlushIncrement 2

bool spFileRotate::getNextFilename(std::string &strFile)
{
    // FS Set?
    if (!_theFS)
        return false;

    char szBuffer[64];
    while (true)
    {
        _currentFileNumber++;

        snprintf(szBuffer, sizeof(szBuffer), "%s%04d.txt", filePrefix.get().c_str(), _currentFileNumber);

        // - does this file already exist?
        if (!_theFS->exists(szBuffer))
            break; // free
    }
    strFile = szBuffer;

    // TODO - save the file name/number to NVR
    return true;
}

//------------------------------------------------------------------------------------------------
// Open the next log file.

bool spFileRotate::openNextLogFile(bool bSendEvent)
{

    if (_currentFile)
    {
        _currentFile.close();
        _currentFile = spFSFile(); // "null file"
        _currentFilename="";
    }
    // Open the next file
    std::string nextFile;
    if (!getNextFilename(nextFile))
        return false;

    _currentFilename = nextFile;

    _currentFile = _theFS->open(_currentFilename.c_str(), spIFileSystem::kFileWrite, true);

    if (!_currentFile)
    {
        spLog_E(F("File Rotate - Unable to create file %s"), _currentFilename.c_str());
        return false;
    }
    _secsFileOpen = spClock.epoch();

    _flushCount = 0; // new file, new start

    // send the new file event.
    if (bSendEvent)
        on_newFile.emit();

    return true;
}

//------------------------------------------------------------------------------------------------
void spFileRotate::write(int value)
{
    write(sp_utils::to_string(value).c_str(), true);
}

//------------------------------------------------------------------------------------------------
void spFileRotate::write(float value)
{
    write(sp_utils::to_string(value).c_str(), true);
}

//------------------------------------------------------------------------------------------------
void spFileRotate::write(const char *value, bool newline)
{

    if (!_theFS)
    {
        spLog_E(F("File Rotate - Unable to output to file. No filesystem set"));
        return;
    }

    // no file - system just starting up?
    if (!_currentFile)
    {
        // open the next file - dont' send an event
        if (!openNextLogFile(false))
            return;
    }
    // Write the current line out
    _currentFile.write((uint8_t *)value, strlen(value) + 1);
    // add a cr if newline set
    if (newline)
        _currentFile.write((uint8_t *)"\n", 1);

    // Will we need to rotate?
    if (spClock.epoch() - _secsFileOpen() > _secsRotPeriod)
    {
        // open the next file, send the new file event. This will cause
        // the next line out to be a "start of the file line" (i.e. header)
        // if that's how the format rolls
        if (!openNextLogFile(true))
            return;
    }

    // flush the file buffer?
    _flushCount = (_flushCount + 1) % kFlushIncrement;
    if (!_flushCount)
        _currentFile.flush();
}

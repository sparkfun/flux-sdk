

#pragma once

#include "spSpark.h"
#include "flxCore.h"
#include "flxCoreInterface.h"
#include "flxFS.h"

// TODO - refactor this out
#include "flxFSSDMMCard.h"
#include <string>

// This object implements the flxWriter interface, and manages the rotation
// of files created on the passed in filesystem.

class flxFileRotate : public flxActionType<flxFileRotate>, public flxWriter
{

  private:
    // rotation period setter/getter
    uint get_RotatePeriod(void)
    {
        return _secsRotPeriod / kSecsPerHour;
    }
    void set_RotatePeriod(uint hours)
    {
        _secsRotPeriod = hours * kSecsPerHour;
    }

  public:
    flxFileRotate() : _currentFilename{""}, _theFS{nullptr}, _flushCount{0}, _secsRotPeriod{0}
    {

        setName("File Rotate",
                "Writes output to a file on the provided filesystem. Rotates files after a given time period.");

        spRegister(rotatePeriod, "Rotate Period", "Time between file rotation.");
        spRegister(startNumber, "File Start Number", "The Number the filename rotation starts with");
        spRegister(filePrefix, "Filename Prefix", "The prefix string for the generated filenames");

        // hidden prop
        spRegister(_secsFileOpen);

        // at startup, current file count == startNumber-1
        _currentFileNumber = startNumber.get() - 1;

        flux.add(this);

    };

    void write(int);
    void write(float);
    void write(const char *, bool newline);

    void setFileSystem(flxIFileSystem *fs)
    {
        if (fs)
            _theFS = fs;
    }
    void setFileSystem(flxIFileSystem &fs)
    {
        setFileSystem(&fs);
    }
    // Rotation Period in Days
    flxPropertyRWUint<flxFileRotate, &flxFileRotate::get_RotatePeriod, &flxFileRotate::set_RotatePeriod> rotatePeriod = {
        24, {{"6 Hours", 6}, {"12 Hours", 12}, {"1 Day", 24}, {"2 Days", 48}, {"1 Week", 168}}};

    flxPropertyUint<flxFileRotate> startNumber = {1};

    flxPropertyString<flxFileRotate> filePrefix = {""};

    // Our "New File" event
    flxSignalVoid on_newFile;

  private:

    // Hidden property - epoch when file was opened...
    flxPropertyHiddenUint<flxFileRotate> _secsFileOpen= {0};

    static constexpr uint kSecsPerHour = 3600;

    bool getNextFilename(std::string &strFile);
    bool openNextLogFile(bool bSendEvent = true);

    std::string _currentFilename;
    flxIFileSystem *_theFS;
    uint8_t _flushCount;
    uint32_t _secsRotPeriod;
    uint _currentFileNumber;

    flxFSFile _currentFile;
};

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

#include "flxCore.h"
#include "flxCoreInterface.h"
#include "flxFS.h"
#include "flxFlux.h"

// TODO - refactor this out
// #include "flxFSSDMMCard.h"
#include <string>

// Define the "new file" event
flxDefineEventID(kOnNewFile);

// This object implements the flxWriter interface, and manages the rotation
// of files created on the passed in filesystem.

class flxFileRotate : public flxActionType<flxFileRotate>, public flxWriter
{

  private:
    // rotation period setter/getter
    uint32_t get_RotatePeriod(void)
    {
        return _secsRotPeriod / kSecsPerHour;
    }
    void set_RotatePeriod(uint32_t hours)
    {
        _secsRotPeriod = hours * kSecsPerHour;
    }

  public:
    flxFileRotate() : _currentFilename{""}, _theFS{nullptr}, _flushCount{0}, _secsRotPeriod{0}, _headerWritten{false}
    {

        setName("File Rotate", "Writes output to a file. Rotates files after a given time period.");

        flxRegister(rotatePeriod, "Rotate Period", "Time between file rotation");
        flxRegister(startNumber, "File Start Number", "The start number for filename rotation");
        flxRegister(filePrefix, "Filename Prefix", "The prefix string for filenames");

        // hidden prop
        flxRegister(_secsFileOpen);
        flxRegister(_currentFileNumber);

        // at startup, current file count == startNumber-1
        _currentFileNumber = startNumber.get() - 1;

        flux_add(this);
    };

    void write(int32_t);
    void write(float);
    void write(const char *, bool newline, flxLineType_t type);

    void setFileSystem(flxIFileSystem *fs)
    {
        if (fs)
            _theFS = fs;
    }
    void setFileSystem(flxIFileSystem &fs)
    {
        setFileSystem(&fs);
    }

    std::string currentFilename(void)
    {
        return _currentFilename;
    }

    // Rotation Period in Days
    flxPropertyRWUInt32<flxFileRotate, &flxFileRotate::get_RotatePeriod, &flxFileRotate::set_RotatePeriod>
        rotatePeriod = {24, {{"6 Hours", 6}, {"12 Hours", 12}, {"1 Day", 24}, {"2 Days", 48}, {"1 Week", 168}}};

    flxPropertyUInt32<flxFileRotate> startNumber = {1};

    flxPropertyString<flxFileRotate> filePrefix = {"sfe"};

    static constexpr const char *kLogFileSuffix = "txt";

  private:
    // Hidden property - epoch when file was opened...
    flxPropertyHiddenUInt32<flxFileRotate> _secsFileOpen = {0};
    flxPropertyHiddenUInt32<flxFileRotate> _currentFileNumber = {0};

    static constexpr uint kSecsPerHour = 3600;
    static constexpr const char *kFileNameTemplate = "%s%04d.%s";

    bool getNextFilename(std::string &strFile);
    bool openNextLogFile();
    bool openCurrentFile(void);
    bool openLogFile(bool bAppend = false);

    std::string _currentFilename;
    flxIFileSystem *_theFS;
    uint8_t _flushCount;
    uint32_t _secsRotPeriod;

    flxFSFile _currentFile;

    bool _headerWritten;
};

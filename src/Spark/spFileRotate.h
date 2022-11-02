

#pragma once

#include "spCoreInterface.h"
#include "spCore.h"
#include "spFSSDCard.h"

#include <string>

// This object implements the spWriter interface, and manages the rotation
// of files created on the passed in filesystem.

class spFileRotate : public spActionType<spFileRotate>, public spWriter
{
public:
	spFileRotate(): _currentFilename{""}, _theFS{nullptr} {

		setName("File Rotate", 
				"Writes output to a file on the provided filesystem. Rotates files after a given time period.");

		spRegister(RotatePeriod, "Rotate Period", "Time between file rotation.");
		spRegister(FileNameStartNumber, "File Start Number", "The Number the filename rotation starts with");
	};

	void write(int );
    void write(float);
    void write(const char *);

    void setFS(spIFileSystem *fs)
    {
    	if (fs)
    		_theFS = fs;
    }

    void setFilename(char * filename)
    {
    	_currentFilename = filename;

    }

    // Rotation Period in Days
    spPropertyUint<spFileRotate>     RotatePeriod={24, {
                                            {"6 Hours", 6},
                                            {"12 Hours", 12},
                                            {"1 Day", 24},        
                                            {"2 Days", 48},      
                                            {"1 Week", 168}                       
                                            }};

    spPropertyUint<spFileRotate>   FileNameStartNumber = {1};

private:

	std::string 	_currentFilename;
	spIFileSystem * _theFS;

	spFSFile   		_currentFile; 


};

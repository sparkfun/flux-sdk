


#include "spFileRotate.h"
#include "spUtils.h"

// number of writes between flushes
#define kFlushIncrement 2

bool spFileRotate::getNextFilename(std::string &strFile)
{
	// FS Set?
	if (!_theFS)
		return false;

	char szBuffer[64];
	while(true)
	{
		_currentFileNumber++;

		snprintf(szBuffer, sizeof(szBuffer), "/%s%04d.txt", filePrefix.get().c_str(), _currentFileNumber);
		
		// - does this file already exist?
		if ( !_theFS->exists(szBuffer)) 
			break; // free

	}
	strFile = szBuffer;

	return true;
}
void spFileRotate::write(int value)
{
	write( sp_utils::to_string(value).c_str());
}

void spFileRotate::write(float value)
{
	write( sp_utils::to_string(value).c_str());
}

void spFileRotate::write(const char * value)
{

	if(!_theFS)
	{
		spLog_E(F("File Rotate - Unable to output to file. No filesystem set"));
		return;
	}

	if (!_currentFile)
	{
		// Open the next file
		std::string nextFile; 
		if (!getNextFilename(nextFile))
			return;

		_currentFilename = nextFile;

		_currentFile = _theFS->open(_currentFilename.c_str(), spIFileSystem::kFileWrite, true);

		if ( !_currentFile)
		{
			spLog_E(F("File Rotate - Unable to create file %s"), _currentFilename.c_str());
			return;
		}
		_ticksFileOpen = millis();
	}

	_currentFile.write((uint8_t*)value, strlen(value)+1);
	// add a cr
	_currentFile.write((uint8_t*)"\n", 1);

	_flushCount = (_flushCount+1)%kFlushIncrement;
	if( !_flushCount)
		_currentFile.flush();

}

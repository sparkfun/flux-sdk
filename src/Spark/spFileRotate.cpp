


#include "spFileRotate.h"
#include "spUtils.h"

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

	if(!_currentFile && !_theFS)
	{
		spLog_E(F("File Rotate - Unable to output to file. No filesystem set"));
		return;
	}

	if (!_currentFile)
	{
		// Open the next file 
		// TODO - use better logic to find file name and manage output

		_currentFile = _theFS->open(_currentFilename.c_str(), spIFileSystem::kFileAppend, true);

		if ( !_currentFile)
		{
			spLog_E(F("File Rotate - Unable to create file %s"), _currentFilename.c_str());
			return;
		}
	}

	_currentFile.write((uint8_t*)value, strlen(value)+1);

}

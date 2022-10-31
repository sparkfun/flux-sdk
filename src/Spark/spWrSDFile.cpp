


#include "spWrSDFile.h"
#include <string.h>

void spWrSDFile::write(int value)
{
	if (!_sdFile.isOpen())
		return;

	// TODO
}

void spWrSDFile::write(float value)
{
	if (!_sdFile.isOpen())
		return;

	//TODO
}
void spWrSDFile::write(const char * value)
{
	if (!_sdFile.isOpen())
		return;

	_sdFile.write(value);

}

bool spWrSDFile::open(char * filename)
{
	if ( !filename || strlen(filename) < 5 )
	{
		spLog_E(F("Invalid filename"));
		return false;
	}
	if (_sdFile.isOpen())
	{
		_sdFile.close();
	}

	// pretty simple 
	return _sdFile.open(filename, O_CREAT | O_APPEND | O_WRITE);
}
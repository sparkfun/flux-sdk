

#pragma once

#include "spCore.h"
#include "spCoreInterface.h"

#include <SdFat.h>  //SdFat by Bill Greiman: http://librarymanager/All#SdFat_exFAT

class spWrSDFile : public spActionType<spWrSDFile>, public spWriter
{
public:
	spWrSDFile(){};
	virtual void write(int);
    virtual void write(float);
    virtual void write(const char *);

    bool open(char *);

private:
	FsFile _sdFile;

};
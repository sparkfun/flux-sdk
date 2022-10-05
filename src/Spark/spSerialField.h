

#pragma once

#include <ctype.h>

#define kEditBufferMax 256



class spSerialField
{

public:
	bool editField(char *value, size_t lenValue, uint32_t timeout=20);

private:
	typedef struct {
    	char head[kEditBufferMax];  // 
    	char tail[kEditBufferMax];  // text after the cursor -  values at:  bcursor to kEditBufferMax-1 ;
    	uint cursor;
    	uint bcursor;
	} FieldContext_t;

	void drawTrailing(FieldContext_t &ctxEdit, bool isDelete=false); 
	void resetContext( FieldContext_t &ctxEdit);
	void processArrowKeys(FieldContext_t &ctxEdit, char inCode);
	void processKillToEOL(FieldContext_t &ctxEdit);
	void processDELKey(FieldContext_t &ctxEdit);
	void processBackspaceKey(FieldContext_t &ctxEdit);
	void processEndOfLineKey(FieldContext_t &ctxEdit);
	void processStartOfLineKey(FieldContext_t &ctxEdit);
	void processText(FieldContext_t &ctxEdit, char * inputBuffer, uint length);
	bool editLoop(FieldContext_t &ctxEdit, uint32_t timeout=10);
};


#pragma once

#include <ctype.h>

#define kEditBufferMax 256



class spSerialField
{

public:
	bool editField(char *value, size_t lenValue, uint32_t timeout=60);

	bool editFieldInt8( int8_t &value, uint32_t timeout=60);
	bool editFieldInt( int32_t &value, uint32_t timeout=60);	
	bool editFieldUInt8( uint8_t &value, uint32_t timeout=60);
	bool editFieldUInt( uint32_t &value, uint32_t timeout=60);	
	bool editFieldFloat( float &value, uint32_t timeout=60);
	bool editFieldDouble( double &value, uint32_t timeout=60);			

private:
	typedef struct {
    	char head[kEditBufferMax];  // 
    	char tail[kEditBufferMax];  // text after the cursor -  values at:  bcursor to kEditBufferMax-1 ;
    	char all[kEditBufferMax];   // internal use 
    	uint cursor;
    	uint bcursor;
    	bool (*validator)(char *value );
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
	void fulltext(FieldContext_t &ctxEdit, char *buffer, size_t length=kEditBufferMax);
};
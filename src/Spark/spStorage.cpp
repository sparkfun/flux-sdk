
/*
 * EEPROM storage system.
 *
 * Data is stored in blocks, with the following structure (n byte - value)
 *
 *  Block:
 *     Header:
 *     	0 - ID
 *     	2 - SIZE - size of storage blob of this block
 *     	4 - NEXT - file offset of the next block
 *     Blob:
 *      6 -> SIZE - the data blob
 * 
 *
 *  Overall format:
 *     The strorage area starts with a 16 bit "magic" code    
 *     0 to N storage blocks 
 *     EOF - end of storage - an zero'd out header block.
 */

#include "spStorage.h"
#include "Arduino.h"
#include <EEPROM.h>


// Define our magic code - what we use to signal that the 
// EEPROM on the device is structured so this system operates
// correctly.
//
// KDB - NOTE FOR ESP32 - future - would need to move this to the "preferance" package.
//    							   EEPROM is deprecated ..

// code we use to determine if EEPROM is setup yet. 
// Why magic -> https://en.wikipedia.org/wiki/Magic_number_(programming)

#define SPARK_STORAGE_MAGIC 0xFA19

// handy macro 
#define BPOS(_block)  _block->_position

// There is only one block transaction open at a time, so we just allocate our block
static spStorageBlock _theBlock={};

//-------------------------------------------------------------------------------
// Constructor

void spStorage_::initialize( void ){

	// Verify that the values in the EEPROM are valid - setup for this
	// system. 

#ifdef ESP32
	EEPROM.begin(200);
#endif 

	if( !this->validStorage()){
		Serial.println("EEPROM - Initializing");
		this->initStorage();
	}
}
//-------------------------------------------------------------------------------
void spStorage_::initStorage(void){

	spBlockHeader header = {}; // zero out header block

	uint16_t buffer = SPARK_STORAGE_MAGIC;
	
	EEPROM.put(0, buffer);
	
	// write the empty block out after the magic - "EOF"
	write_bytes(sizeof(header), sizeof(header), (char*)&header);

#ifdef ESP32
	EEPROM.commit();
#endif
}
//-------------------------------------------------------------------------------
bool spStorage_::validStorage(void){

	// Basically make sure the value of the first two bytes of EEPROM are 
	// set to our MAGIC 
	uint16_t cookie;

	EEPROM.get(0, cookie);
	
	return (cookie == SPARK_STORAGE_MAGIC);

}
//--------------------------------------------------------------------------------------
uint16_t spStorage_::findBlock(uint16_t idTarget, spBlockHeader &outBlock){

	// is the storage *formated*
	if(!this->validStorage()){
		this->initStorage(); 
	}

	// walk the storage blocks until the end block is found, or our
	// target block is found
	uint16_t currPosition;

	// Read the block headers to find either the end block (id=0),
	// or our target bock (id == idTarget)

	outBlock.next = sizeof(uint16_t); // seed the loop - next block is after magic

	while(true){

		currPosition = outBlock.next;
		read_bytes(currPosition, outBlock);	

		if(!outBlock.id || outBlock.id == idTarget)
			break;
	}
	return currPosition;
}

//--------------------------------------------------------------------------------------
// deleteBlock()
//
// Removes a block from storage area. Basically overwrites the block with
// the data blocks that follows it.
//
void spStorage_::deleteBlock(uint16_t idTarget){

	spBlockHeader destBlock, srcBlock;
	uint16_t destPosition = findBlock(idTarget, destBlock); // find target block

	if(!destBlock.id || destBlock.id != idTarget)  // not there?
		return; 
	
	uint16_t srcPosition = destBlock.next; // where to start pulling data from
	while(srcPosition){

		read_bytes(srcPosition, srcBlock);

		destBlock = srcBlock;
		if(destBlock.id) // not at EOF
			destBlock.next = destPosition + sizeof(spBlockHeader) + destBlock.size;
		write_bytes(destPosition, destBlock);

		// copy over the data
		if(destBlock.size){
			char szBuffer[destBlock.size];
			read_bytes(srcPosition + sizeof(spBlockHeader), destBlock.size, (char*)szBuffer);
			write_bytes(destPosition + sizeof(spBlockHeader), destBlock.size, (char*)szBuffer);			
		}
		destPosition = destBlock.next;
		srcPosition = srcBlock.next;

	}
#ifdef ESP32
	EEPROM.commit();
#endif
}
//--------------------------------------------------------------------------------------
// getBlockHeader()
//
// Finds/creates block with the given ID and size
//
// Returns 0 on error, or offset position of the block on success.
//
uint16_t spStorage_::getBlockHeader(uint16_t idTarget, size_t szBlock, spBlockHeader &outBlock){

	// Find the block or eof (empty block)
	spBlockHeader currBlock;
	uint16_t currPosition = findBlock(idTarget, currBlock);

	// Found the target block?
	if(currBlock.id){  

		// Not enough storage?
		if(szBlock  && szBlock > currBlock.size){

			// Okay, the match block was found, BUT the allocation is too 
			// small. Need to delete this block and then get a new block of the
			// correct block size allocated. 
			//
			// Delete the block, and recurse - which allocates a new block at the end of
			// the area with the correct size. 

			deleteBlock(idTarget);
			return getBlockHeader(idTarget, szBlock, outBlock);
		}

		outBlock = currBlock;

	}else {	 // At EOF (a block didn't aready exist )
		
		// Set new block parameters/values
		outBlock.id   = idTarget;
		outBlock.size = szBlock;
		outBlock.next = currPosition + sizeof(spBlockHeader) + szBlock;

		// Allocate the block in the eeprom - write out with values
		write_bytes(currPosition, outBlock);

		// Set our eof empty block
		memset(&currBlock, '\0', sizeof(spBlockHeader));
		write_bytes(outBlock.next, currBlock);

#ifdef ESP32
		EEPROM.commit();
#endif
	}

	return currPosition;

}
//-------------------------------------------------------------------------------
// Internal 
template<typename T>
void spStorage_::write_bytes(uint16_t startPos, T& data){

	write_bytes(startPos, sizeof(T), (char*)&data);
}
//-------------------------------------------------------------------------------
void spStorage_::write_bytes(uint16_t startPos, size_t sz, char * pBytes){

	if(!pBytes)
		return;

	for(int i=0; i < sz; i++, startPos++)
		EEPROM.write(startPos, *(pBytes+i));

}

//-------------------------------------------------------------------------------
template<typename T>
void spStorage_::read_bytes(uint16_t startPos, T& data){

	read_bytes(startPos, sizeof(T), (char*)&data);
}
//-------------------------------------------------------------------------------
void spStorage_::read_bytes(uint16_t startPos, size_t sz, char * pBytes){

	if(!pBytes)
		return;

	for(int i=0; i < sz; i++, startPos++)
		*(pBytes+i) = EEPROM.read(startPos);		

}
////////////////////////////////////////////////////////////////////////////////////
// Public write to block methods (not an entire block at a time transaction,
// but start a block and write to it N times, in sequence.
////////////////////////////////////////////////////////////////////////////////////

spStorageBlock * spStorage_::beginBlock(uint16_t blockID, size_t blockSZ){

	if(_theBlock._locked){
		// block in use
		Serial.println("I/O Error: Storage block in use");
		return nullptr;
	}

	_theBlock._position = this->getBlockHeader(blockID, blockSZ, _theBlock.header);

	if(!_theBlock._position){
		return nullptr;
	}

	_theBlock._position += sizeof(spBlockHeader); // start of blob space for block
	_theBlock.header.id = blockID;
	_theBlock._locked = true;
	
	return &_theBlock;

}
//-------------------------------------------------------------------------------
// Done with the block
void spStorage_::endBlock(spStorageBlock* dummy){

	_theBlock._locked=false;

#ifdef ESP32
	EEPROM.commit();
#endif
}


// Block Public methods - these are called from a block
//-------------------------------------------------------------------------------
// I/O routines - simple

bool spStorage_::writeBytes(spStorageBlock* pBlock, size_t sz, char * pBytes){

	if(!pBlock || !pBytes)
	   return false;

	write_bytes(BPOS(pBlock), sz, pBytes);

	BPOS(pBlock) += sz;

	return true;
}
//-------------------------------------------------------------------------------	
bool spStorage_::readBytes(spStorageBlock* pBlock, size_t sz, char* pBytes){

	if(!pBlock || !pBytes)
	   return false;

	read_bytes(BPOS(pBlock), sz, pBytes);

	BPOS(pBlock) += sz;
	return true;
}	
//-------------------------------------------------------------------------------	
//-------------------------------------------------------------------------------	
bool spStorageBlock::writeBytes(size_t sz, char * buffer){

		return spStorage().writeBytes(this, sz, buffer);
}
//------------------------------------------------------------------------------
bool spStorageBlock::readBytes(size_t sz, char * buffer){

		return spStorage().readBytes(this, sz, buffer);
}
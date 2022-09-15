



#include "spCore.h"
#include "spStorage.h"


//////////////////////////////////////////////////////////////////////
// Object Property Interface 
//////////////////////////////////////////////////////////////////////
//
// get/set property 
template< typename T > 
bool spIProperty::getProperty(const char* name, T&  value){

	if(!name || !_myProps.size())
		return false;

	for(auto property: _myProps){
		if(!strcmp(name, property->name)){
			value = (T)property;
			return true;
		}
	}

}
//-----------------------------------------------------------------
template< typename T > 
bool spIProperty::setProperty(const char* name, T&  value){

	if(!name || !_myProps.size())
		return false;

	for(auto property: _myProps){		
		if(!strcmp(name, property->name)){
			property = (T)value;
			return true;
		}
	}
}

//-----------------------------------------------------------------
bool spIProperty::save(spStorageBlock *sBLK){
	
	for(auto property: _myProps)			
		property->save(sBLK);
	
	return true;
}
//-----------------------------------------------------------------
bool spIProperty::restore(spStorageBlock *sBLK){

	for(auto property: _myProps)				
		property->restore(sBLK);
	
	return true;

}
//-----------------------------------------------------------------
size_t spIProperty::save_size(void){

	size_t sz=0;

	for(auto property: _myProps)					
		sz += property->save_size();

	return sz;
}	


////////////////////////////////////////////////////////////////////////////////////////
// Internal hash function used to generate a unique ID based on a string
//    From: http://www.cse.yorku.ca/~oz/hash.html

static uint16_t id_hash(const char* str){

	uint32_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return  hash & 0xFFFF; // NOTE - we're just using 16 bits 

}
////////////////////////////////////////////////////////////////////////////////////////
// Define a simple class hierarchy interface definitions. Used to walk the hierarchy.
////////////////////////////////////////////////////////////////////////////////////////
//
// spBase - base class
//
// Counter to help with object naming
uint16_t spBase::_name_count=1;

void spBase::onPropertyUpdate(const char *){

	//TODO Did the name change? If so, we need to update the ID.

	// NOOP ...
}

//---------------------------------------------------------------------------
// getID()
//
// Return a unique id for the class - used with serliazation...
//
uint16_t spBase::getID(void){

	if(!_id){
		if(!name.size()){ // no name, make one
			char szBuffer[12];
			snprintf(szBuffer, sizeof(szBuffer), "spark%03d", _name_count++);
			name = szBuffer;
		}
		// hash the name - should be unique
		_id = id_hash(name);
	}
	return _id;
}

bool spBase::save(void){

	int id = getID();  // object id.

	size_t sz = spIProperty::save_size();

	if( sz > 0){
		spStorageBlock *BLK = spStorage().beginBlock(id, sz);
		bool rc = spIProperty::save(BLK); // TODO: Check errors
		spStorage().endBlock(BLK);
	}
	
	return true;
}
//-----------------------------------------------------------------
bool spBase::restore(void){

	// TODO: something isnt fully working here
	//Serial.print("BASE RESTORE: "); Serial.println(this->getName());
	uint16_t id = getID();  // object id.

	spStorageBlock *BLK = spStorage().beginBlock(id, 0); // 0 size = use what's in the repo
	Serial.println("a");
	bool rc = spIProperty::restore(BLK);
	Serial.println("b");		
	spStorage().endBlock(BLK);
	Serial.println("c");		

	return true;
}

//-----------------------------------------------------------------
bool spBase::serializeJSON(JsonObject &jObj){

	// Place the property values + the object name in the Json Object prrovided.
	jObj["id_debug"] = getID();

	for( auto prop: _myProps){
		// Use a method to get the value and set in the JSON document. 
		// The getValue method allows the template to define a type at compile
		// time that works with the template of the Json variable
		prop->getValue(jObj.getOrAddMember(prop->name));
	}

	return true;
}

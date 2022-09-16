/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"


void spLogger::logParameters(spDataCoreList& params, spObservation &theObs){

	for( auto op : params){	

		switch(op->type()){
 		case TypeBool: case TypeInt:	
			theObs.logValue(op->name, op->getInt());
 			break;

 		case TypeFloat: case TypeDouble:
 			theObs.logValue(op->name, op->getFloat());
 			break;

 		case TypeString:
 			theObs.logValue(op->name, op->getString()); 				
 			break;
 		default:
 			Serial.println("Unknown Parameter Value");
 			break;
		}
	}

}
//----------------------------------------------------------------------------
void spLogger::logObservation(void){

	// loop over our attached devices and log current readings/value	
	spObservation theObs;	

	// if we have general params to log, do those.
	if(_paramsToLog.size() > 0 ){

		theObs.beginSection("general"); //TODO: Better name?

		logParameters(_paramsToLog, theObs);
	}

	// loop over objs to log - each object is in a named section.
	// TODO: probalby make sure names have a legal syntax.
	for( auto pObj : _objsToLog){

		theObs.beginSection(pObj->name);	

		// loop over the output parameters of the device and log values
		logParameters(pObj->getOutputParameters(), theObs);
	}
	
	// Okay, now output the observation to our writers
	for( auto outputDev : _Writers)
		outputDev->write(theObs);	
}

/*
 *---------------------------------------------------------------------------------
 *
 *---------------------------------------------------------------------------------
 */

#include "spLogger.h"


void spLogger::logParameters(spDataCoreList& params, spOutputFormat *theFormatter){

	for( auto op : params){	

		switch(op->type()){
 		case TypeBool: case TypeInt:	
			theFormatter->logValue(op->name, op->getInt());
 			break;

 		case TypeFloat: case TypeDouble:
 			theFormatter->logValue(op->name, op->getFloat());
 			break;

 		case TypeString:
 			theFormatter->logValue(op->name, op->getString()); 				
 			break;
 		default:
 			Serial.println("Unknown Parameter Value");
 			break;
		}
	}

}
//----------------------------------------------------------------------------
void spLogger::logObservation(void){

	// for each output format, loop over our attached devices and log current readings/value	


	for ( auto theFormatter: _Formatters)
	{

		theFormatter->beginObservation();


		// if we have general params to log, do those.
		if(_paramsToLog.size() > 0 )
		{
			theFormatter->beginSection("General"); //TODO: Better name?

			logParameters(_paramsToLog, theFormatter);
		}

		// loop over objs to log - each object is in a named section.
		// TODO: probalby make sure names have a legal syntax.
		for( auto pObj : _objsToLog)
		{
			theFormatter->beginSection(pObj->name);	

			// loop over the output parameters of the device and log values
			logParameters(pObj->getOutputParameters(), theFormatter);
		}

		theFormatter->endObservation();

		// Write out this observation and clear it out
		theFormatter->writeObservation();
		theFormatter->clearObservation();

	}
}


#pragma once 

#include <Arduino.h>

#include "spSpark.h"
 
class spTimer: public spAction {

	// for our timing 
	unsigned long _lastLogTime=0;

public:

	spTimer(int start=500):_lastLogTime(0){
	
		spRegisterProperty(interval, start);	

		spark.add(this);

		name="Timer";
	};

	// Our output event
	spSignalVoid on_interval; 

	// Property  - interval for the timer
	spPropertyInt interval; 

	bool loop(void){
		// Is it time to log - Delta since last log > Log Interval?? 
		if(millis() - _lastLogTime < interval)
			return false; // we didn't do anything
	
		// Reset our last logged time to now
		_lastLogTime = millis();

		// trigger the event
		on_interval.emit();

		return true;
	}
};

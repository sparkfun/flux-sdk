/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */
 
/*
 * Spark Framework demo - logging
 *   
 */

// Spark framework 
#include "sfeDataLogger.h"



#define OPENLOG_ESP32
#ifdef OPENLOG_ESP32
#define EN_3V3_SW 32
#define LED_BUILTIN 25
#endif


// Our data logger application

sfeDataLogger  theDataLogger;

//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);


#ifdef OPENLOG_ESP32    
    pinMode(EN_3V3_SW, OUTPUT); // Enable Qwiic power and I2C
    digitalWrite(EN_3V3_SW, HIGH);
#endif

    // Start up the framework
    flux.start();

    digitalWrite(LED_BUILTIN, LOW);  // board LED off

}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    ///////////////////////////////////////////////////////////////////
    // Spark
    //
    // Just call the spark framework loop() method. Spark will manage
    // the dispatch of processing to the components that were added 
    // to the system during setup.
    if(flux.loop())        // will return true if an action did something
        digitalWrite(LED_BUILTIN, HIGH); 

    // Our loop delay 
    delay(500); 
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

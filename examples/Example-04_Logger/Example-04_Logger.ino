/*
 * Spark Framework demo - logging
 *   
 */

// Spark framework 
#include <Spark.h>
#include <Spark/spLogger.h>
#include <Spark/spTimer.h>


//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 6000

/////////////////////////////////////////////////////////////////////////
// Spark Framework
/////////////////////////////////////////////////////////////////////////
// Spark Structure and Object Definition
//
// This app implements a "logger", which grabs data from 
// connected devices and writes it to the Serial Console 

// Create a logger action and add: 
//   - Output devices: Serial 

// Note - these could be added later using the add() method on logger

spLogger  logger(spSerial());

// Enable a timer with a default timer value - this is the log interval
spTimer   timer(kDefaultLogInterval);    // Timer 

// Setup and start the framework objects. 
//---------------------------------------------------------------------
// app_setup()
//
// function that assembles the framework objects to build the
// opertional structure of this application.
//
// For this example:
//  - The logger connects to the timer event.
//  - Any detected devices are added to the logger
//      

bool app_setup(void){

    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    spark.start();  

    // Logging is done at an interval - using an interval timer. 
    // Connect logger to the timer event
    logger.listen(timer.on_interval);  

    // What devices has the system detected?

    for(auto device: spark.connectedDevices()){

        Serial.print("Adding Device: "); Serial.println(device->name);
        logger.add(device);
    }

    return true;
}
/////////////////////////////////////////////////////////////////////////
// End Spark
/////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup ----");
    
    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    spark.start();  

    // Logging is done at an interval - using an interval timer. 
    // Connect logger to the timer event
    logger.listen(timer.on_interval);  

    // What devices has the system detected?
    // List them and add them to the logger

    for(auto device: spark.connectedDevices()){

        Serial.print("Adding Device: "); Serial.println(device->name);
        logger.add(device);
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    ///////////////////////////////////////////////////////////////////
    // Spark
    //
    // Just call the spark framework loop() method. Spark will maanage
    // the dispatch of processing to the components that were added 
    // to the system during setup.
    if(spark.loop())        // will return true if an action did something
        digitalWrite(LED_BUILTIN, HIGH); 

    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

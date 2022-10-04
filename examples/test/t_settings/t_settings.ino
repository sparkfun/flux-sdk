/*
 * Spark Framework demo - logging
 *   
 */

// Spark framework 
#include <Spark.h>
#include <Spark/spLogger.h>
#include <Spark/spFmtJSON.h>
#include <Spark/spFmtCSV.h>
#include <Spark/spTimer.h>
#include <Spark/spSerial.h>

// KDB TESTING
#include <Spark/spSettingsSerial.h>





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

// Create a JSON and CSV output formatters. 
// Note: setting internal buffer sizes using template to minimize alloc calls. 


spLogger  logger;

// Enable a timer with a default timer value - this is the log interval
spTimer   timer(3000);    // Timer 

spSettingsSerial serialSettings;
//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup Serial Settings Test ----");
    
    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    spark.start();  

    
    // What's connected
    spDeviceContainer  myDevices = spark.connectedDevices();

    Serial.printf("Number of Devices Detected: %d\r\n", myDevices.size() );

    // Loop over the device list - note that it is iterable. 
    for (auto device: myDevices )
    {
        Serial.printf("Device: %s, Output Number: %d\n\r", device->name(), device->nOutputParameters());
        
    }

    
    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    // Set the settigns system to start at root of the spark system.
    serialSettings.setSystemRoot(&spark);

    // Add serial settings to spark - the spark loop call will take care
    // of everything else.
    spark.add(serialSettings);
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
    spark.loop();

    // Our loop delay 
    delay(500);                       
    //digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    //delay(500);
}

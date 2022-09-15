/*
 * Spark Framework demo - simple output
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>
#include <Spark/spDevBME280.h>
#include <Spark/spDevCCS811.h>

/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

spDevBME280 myBME;
spDevCCS811 myCCS;

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
    Serial.println("HELLO HELLO");
    
    // Wire startup
    Wire.begin();
    // Setup Spark, but don't have the framework autoload devices
    spark.start(false);

    

    // Initialize our devices

    if (!myBME.initialize(Wire) )
    {
        Serial.println("[Error] - Startup of BME280 failed. Halting");
        while(1);
    }
    if (!myCCS.initialize(Wire) )
    {
        Serial.println("[Error] - Startup of CCS811 failed. Halting");
        while(1);
    }
    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    // Retrieve the data from the devices.

    digitalWrite(LED_BUILTIN, HIGH);   // turn on the log led    
    Serial.print("Device: "); Serial.println(myBME.name);
    //Serial.print("   Temp F   : "); Serial.println(myBME.temperature_f.getFloat(); 
    //Serial.print("   Humidity : "); Serial.println(myBME.humidity());     

    Serial.print("Device: "); Serial.println(myCCS.name);
    // Serial.print("   CO2   : "); Serial.println(myCCS.co2()); 
    // Serial.print("   TVOC  : "); Serial.println(myCCS.tvoc());  

    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

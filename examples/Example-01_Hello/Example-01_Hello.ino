/*
 * Spark Framework demo - simple output
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>
#include <Spark/flxDevBME280.h>
#include <Spark/flxDevCCS811.h>

/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

flxDevBME280 myBME;
flxDevCCS811 myCCS;

bool BMEIsConnected=false;
bool CCSIsConnected=false;
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
    
    // Wire startup
    Wire.begin();

    // Setup Spark, but don't have the framework autoload devices
    spark.start(false);

    
    // init devices
    if (!myBME.initialize() )
    {
        Serial.println("[Error] - Startup of BME280 failed. Halting");
        while(1);
    }else {
        Serial.println("BME280 initialized");
        BMEIsConnected=true;
    }



    if (!myCCS.initialize() )
    {
        Serial.println("[Error] - Startup of CCS811 failed. Halting");
        while(1);

    }else {
        Serial.println("CCS811 initialized");    
        CCSIsConnected = true;
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    // Retrieve the data from the devices.
    digitalWrite(LED_BUILTIN, HIGH);   // turn on the log led    

    Serial.print("Device: "); Serial.println(myBME.name());
    if (BMEIsConnected)
    {
        Serial.print("   Temp F   : "); Serial.println(myBME.temperatureF()); 
        Serial.print("   Humidity : "); Serial.println(myBME.humidity());     
    } 
    else 
        Serial.println("Not Connected.");
    

    Serial.print("\nDevice: "); Serial.println(myCCS.name());

    if (CCSIsConnected)
    {
        Serial.print("   CO2   : "); Serial.println(myCCS.co2()); 
        Serial.print("   TVOC  : "); Serial.println(myCCS.tvoc());  
    } else
        Serial.println("Not Connected.");

    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

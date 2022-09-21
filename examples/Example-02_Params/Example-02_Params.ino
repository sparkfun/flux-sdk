/*
 * Spark Framework demo - simple output
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>

/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// Uses parameter instrospection to print output values of the BME280
// 
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

spDevBME280 myBME;

bool BMEIsConnected=false;

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

    // Setup Spark - this triggers an autoload, which should skip over the BME280
    spark.start();

    // init devices
    if (!myBME.initialize() )
    {
        Serial.println("[Error] - Startup of BME280 failed");
    }else {
        Serial.println("BME280 initialized");
        BMEIsConnected=true;
    }

    myBME.celsius = false;
    bool bb = myBME.celsius;

    bb = !bb;

    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    // Retrieve the data from the devices.
    digitalWrite(LED_BUILTIN, HIGH);   // turn on the log led    

    Serial.print("Device: "); Serial.println(myBME.name);
    if (BMEIsConnected){

        // Loop over the output parameters of this device and print name and value
        for (auto param : myBME.getOutputParameters() )
        {

            Serial.print(" "); Serial.print(param->name); Serial.print(" = ");

            switch(param->type()){
                case TypeBool: case TypeInt:    
                    Serial.println(param->getInt());
                    break;

                case TypeFloat: case TypeDouble:
                    Serial.println(param->getFloat());
                    break;

                case TypeString:
                    Serial.println(param->getCString());                 
                    break;
                default:
                    Serial.println("Unknown Parameter Type");
                    break;
            }
        }

    } 
    else 
        Serial.println("Not Connected.");
    
    Serial.println();
    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

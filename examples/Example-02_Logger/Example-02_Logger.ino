/*
 * Spark Framework demo - logging
 *   
 */


#include <WiFiClientSecure.h>
#include "WiFi.h"

// Spark framework 
#include <Spark.h>
#include <Spark/spDevBME280.h>
#include <Spark/spDevCCS811.h>

// secrets for the system 
#include "wifi_secrets.h"

//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 6000

//------------------------------------------
// System objects
// 
// Secure network connection (ssl)
WiFiClientSecure secureNetwork = WiFiClientSecure();

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
// setupWiFi()
// Connect to network setup secure connection

void setupWiFi() {

    if (!strlen(kWiFiSSID) ){
        Serial.println("[Info] - WiFi Disabled - No Network Parameters Specified.");
        return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin(kWiFiSSID, kWiFiPassword);
    
    Serial.print("WiFi...");
    
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print('.');
    }
    Serial.println(" Connected");
    
}


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
    
    // Connect to WiFi
    setupWiFi();

    // setup the framework for this system. 
    if(!app_setup()){
        Serial.println("Error setting up Spark");
        while(1); // No reason to continue -> die
    }

    //devBME.initialize(Wire);

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

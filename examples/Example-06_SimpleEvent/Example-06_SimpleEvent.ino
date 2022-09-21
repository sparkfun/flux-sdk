/*
 * Spark Framework demo - Simple Events
 *   
 */

// Spark framework 
#include <Spark.h>
#include <Spark/spDevButton.h>
#include <Spark/spSerial.h>



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


    spDevTwist *pTwist = spark.get<spDevTwist>();

    if(pTwist)
    {
        // dump twist messages to serial
        spSerial()->listen( pTwist->on_twist );
        spSerial()->listen( pTwist->on_clicked);

        Serial.println("Twist Connected");
    }
    else 
        Serial.println("No Twist Connected.");

    spDevButton *pButton = spark.get<spDevButton>();

    if(pButton){
        // dump button messages to serial
        spSerial()->listen( pButton->on_clicked);
        Serial.println("Button Connected");
    }
    else 
        Serial.println("No Button Connected.");
  
    if (!pButton && !pTwist )
    {
        Serial.println("No devices connected. Ending");
        while(1);
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    Serial.printf("\n\rSetup Finished\n\r");
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
    delay(100);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(100);
}

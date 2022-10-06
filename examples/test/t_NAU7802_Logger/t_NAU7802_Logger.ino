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

// get testing
#include <Spark/spDevNAU7802.h>


//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 1000

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

// Create a CSV output formatter. 
// Note: setting internal buffer sizes using template to minimize alloc calls. 
spFormatCSV fmtCSV;

spLogger  logger;

// Enable a timer with a default timer value - this is the log interval
spTimer   timer(kDefaultLogInterval);    // Timer 

//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    delay(1000);

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup ----");
    
    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    spark.start();  

    // Logging is done at an interval - using an interval timer. 
    // Connect logger to the timer event
    logger.listen(timer.on_interval);  

    // We want to output CSV to the serial console.
    //  - Add Serial to our  formatters
    fmtCSV.add(spSerial());    

    //  - Add the CSV format to the logger
    logger.add(fmtCSV);    

    // What devices has the system detected?
    // List them and add them to the logger

    spDeviceContainer  myDevices = spark.connectedDevices();

    // The device list can be added directly to the logger object using an 
    // add() method call. This will only add devices with output parameters. 
    //
    // Example:
    //      logger.add(myDevices);   
    //
    // But for this example, let's loop over our devices and show how use the
    // device parameters.

    Serial.printf("Number of Devices Detected: %d\r\n", myDevices.size() );

    // Loop over the device list - note that it is iterable. 
    for (auto device: myDevices )
    {
        Serial.printf("Device: %s, Output Number: %d", device->name(), device->nOutputParameters());
        if ( device->nOutputParameters() > 0)
        {
            Serial.printf("  - Adding to logger\r\n");
            logger.add(device);
        }
        else
            Serial.printf(" - Not adding to logger \r\n");
    }

    ////////////
    // get() testing
    auto devs = spark.get<spDevNAU7802>();

    Serial.printf("Number of NAU7802 devices: %d \n\r", devs->size());
    for( auto dev : *devs)
    {
        Serial.printf("Device name: %s", dev->name());
    }

    ////////////
    // temporary fix so we can zero the NAU7802 and calibrate it to "100"
    while (Serial.available())
      Serial.read(); // Clear the Serial buffer

    /// END TESTING
    
    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    Serial.printf("\n\rLog Output:\n\r");
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

    ////////////
    // temporary fix so we can zero the NAU7802 and calibrate it to "100"
    if (Serial.available())
    {
      char c = Serial.read(); // Read the character

      auto devs = spark.get<spDevNAU7802>();

      // TO DO: figure out how to call begin or front correctly...
      // What I want to do is something like:
      // auto dev = devs.begin(); // Select the first NAU7802
      for ( auto dev : *devs ) // Cheat... Only works because - at the moment - there can only be one NAU7802 attached...
      {
        if ((c == 'z') || (c == 'Z')) // Zero the scale
        {
          dev->calculateZeroOffset(true);
        }
  
        if ((c == 'h') || (c == 'H')) // Calibrate to 100
        {
          dev->calculateCalibrationFactor(100.0);
        }
      }
    }
    /// END TESTING

    // Our loop delay 
    delay(100);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
}

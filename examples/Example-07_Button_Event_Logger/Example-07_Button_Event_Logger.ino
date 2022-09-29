/*
 * Spark Framework demo - event logging
 *   
 */

// Spark framework 
#include <Spark.h>
#include <Spark/spLogger.h>
#include <Spark/spFmtCSV.h>
#include <Spark/spSerial.h>
#include <Spark/spDevButton.h>


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

// Create a CSV output formatters. 
// Note: setting internal buffer sizes using template to minimize alloc calls. 
spFormatCSV fmtCSV;

spLogger  logger;

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

    // We want to output CSV to the serial consol.
    //  - Add Serial to our  formatters
    fmtCSV.add(spSerial());    

    //  - Add the JSON and CVS format to the logger
    logger.add(fmtCSV);    

    // What devices has the system detected?
    // List them

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
            Serial.printf(" - Not adding to logger\r\n");
            
        if ( strcmp(device->name(), "button") == 0 ) // Have we detected a button?
            Serial.printf("Hey! This one is a button! It is using address 0x%02x\r\n", device->address());
    }

    spDevButton *pButton = spark.get<spDevButton>();

    if(pButton)
    {
        pButton->pressMode = false; // Change from Press Mode to Click (Toggle) Mode
        
        // Connect logger to the clicked event
        logger.listen(pButton->on_clicked_event);
        Serial.println("Button Connected");
    }
    else 
        Serial.println("No Button Connected.");

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

    // Our loop delay 
    delay(10);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(10);
}

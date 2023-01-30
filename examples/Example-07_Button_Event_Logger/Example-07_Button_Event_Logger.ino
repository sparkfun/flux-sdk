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
 * Spark Framework demo - event logging
 *   
 */

// Spark framework 
#include <Flux.h>
#include <Flux/flxLogger.h>
#include <Flux/flxFmtCSV.h>
#include <Flux/flxSerial.h>

#include <Flux/flxDevButton.h> // For get testing
#include <Flux/flxDevTwist.h> // For get testing


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
flxFormatCSV fmtCSV;

flxLogger  logger;

//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    delay(1000);

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup ----");
    
    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    flux.start();  

    // We want to output CSV to the serial console.
    //  - Add Serial to our formatters
    fmtCSV.add(flxSerial());    

    //  - Add the CSV format to the logger
    logger.add(fmtCSV);    

    // What devices has the system detected?
    // List them

    flxDeviceContainer  myDevices = flux.connectedDevices();

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
    }

    // Identify any buttons
    // Add their on_clicked events as logger triggers
    auto buttons = flux.get<flxDevButton>();
    
    Serial.printf("Number of buttons: %d\r\n", buttons->size());

    if (buttons->size() > 0)
    {
        for( auto b : *buttons)
        {
            Serial.printf("Connecting the logger to the button named %s at address 0x%02X\r\n", b->name(), b->address());
            
            b->pressMode = false; // Change the button from Press Mode to Click (Toggle) Mode
            b->ledBrightness = 8; // Change the LED brightness from 128 (default) to 8

            logger.listen(b->on_clicked); // Connect logger to the clicked event
        }
    }

    // Identify any Qwiic Twists
    // Add their on_clicked and on_twist events as logger triggers
    auto twists = flux.get<flxDevTwist>();
    
    Serial.printf("Number of twists: %d\r\n", twists->size());

    if (twists->size() > 0)
    {
        for( auto t : *twists)
        {
            Serial.printf("Connecting the logger to the twist named %s at address 0x%02X\r\n", t->name(), t->address());
            
            t->pressMode = false; // Change the mode from Press Mode to Click (Toggle) Mode

            logger.listen(t->on_clicked); // Connect logger to the clicked event
            logger.listen(t->on_twist); // Connect logger to the twist event
        }
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    Serial.printf("\n\rLog Output:\n\r");
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
    delay(50);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
}

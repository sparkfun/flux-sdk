/*
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 */

/*
 * Flux Framework demo - logging
 *
 */

// Spark framework
#include <Flux.h>
#include <Flux/flxFmtCSV.h>
#include <Flux/flxFmtJSON.h>
#include <Flux/flxLogger.h>
#include <Flux/flxSerial.h>
#include <Flux/flxTimer.h>

#include "test_device.h"

//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 10000

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
flxFormatJSON<4000> fmtJSON;
flxFormatCSV fmtCSV;

flxLogger logger;

// Enable a timer with a default timer value - this is the log interval
flxTimer timer(kDefaultLogInterval); // Timer

// create our test device.
test_device myTestDevice;
//---------------------------------------------------------------------
// Arduino Setup
//
void setup()
{

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("\n---- Startup ----");

    // start framework
    flux.start(false); // pass in false flag - prevents auto-load

    // Logging is done at an interval - using an interval timer.
    // Connect logger to the timer event
    logger.listen(timer.on_interval);

    // We want to output JSON and CSV to the serial consol.
    //  - Add Serial to our  formatters
    fmtJSON.add(flxSerial());
    fmtCSV.add(flxSerial());

    //  - Add the JSON and CVS format to the logger
    logger.add(fmtJSON);
    logger.add(fmtCSV);

    // Add the test device to the logger
    logger.add(myTestDevice);

    digitalWrite(LED_BUILTIN, LOW); // board LED off

    Serial.printf("\n\rLog Output:\n\r");
}

//---------------------------------------------------------------------
// Arduino loop -
void loop()
{

    ///////////////////////////////////////////////////////////////////
    // Spark
    //
    // Just call the spark framework loop() method. Spark will manage
    // the dispatch of processing to the components that were added
    // to the system during setup.
    if (flux.loop()) // will return true if an action did something
        digitalWrite(LED_BUILTIN, HIGH);

    // Our loop delay
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW); // turn off the log led
    delay(1000);
}

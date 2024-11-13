/*
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 */

/*
 * Flux Framework demo - simple events
 *
 * Note: This demo requires the generation of a SparkFun_Flux Arduino library - that is then installed
 *      into the Arduino libraries folder, or via the --library option in the Arduino CLI.
 *
 */

// Spark framework
#include <Flux.h>
#include <Flux/flxDevButton.h>
#include <Flux/flxDevTwist.h>
#include <Flux/flxSerial.h>

/////////////////////////////////////////////////////////////////////////
// Flux Framework
/////////////////////////////////////////////////////////////////////////
// Flux Structure and Object Definition
//
// This app implements a "logger", which grabs data from
// connected devices and writes it to the Serial Console

// Create a logger action and add:
//   - Output devices: Serial

// Note - these could be added later using the add() method on logger

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

    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    flux.start();

    auto theTwists = flux.get<flxDevTwist>();

    for (auto pTwist : *theTwists)
    {
        // dump twist messages to serial
        flxSerial()->listen(pTwist->on_twist);
        flxSerial()->listen(pTwist->on_clicked);

        Serial.println("Twist Connected");
    }

    if (theTwists->size() == 0)
        Serial.println("No Twist Connected.");

    auto theButtons = flux.get<flxDevButton>();

    for (auto pButton : *theButtons)
    {
        // dump button messages to serial
        flxSerial()->listen(pButton->on_clicked);
        Serial.println("Button Connected");
    }

    if (theButtons->size() == 0)
        Serial.println("No Button Connected.");

    if (theTwists->size() == 0 && theButtons->size() == 0)
    {
        Serial.println("No devices connected. Ending");
        while (1)
            ;
    }

    digitalWrite(LED_BUILTIN, LOW); // board LED off

    Serial.printf("\n\rSetup Finished\n\r");
}

//---------------------------------------------------------------------
// Arduino loop -
void loop()
{

    ///////////////////////////////////////////////////////////////////
    // Spark
    //
    // Just call the spark framework loop() method. Spark will maanage
    // the dispatch of processing to the components that were added
    // to the system during setup.
    if (flux.loop()) // will return true if an action did something
        digitalWrite(LED_BUILTIN, HIGH);

    // Our loop delay
    delay(100);
    digitalWrite(LED_BUILTIN, LOW); // turn off the log led
    delay(100);
}

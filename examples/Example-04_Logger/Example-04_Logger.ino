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

// Testing for device calls
#include <Spark/spDevButton.h>

// SD Card output
#include <Spark/spFSSDCard.h>
#include <Spark/spFileRotate.h>

#define OPENLOG_ESP32
#ifdef OPENLOG_ESP32
#define EN_3V3_SW 32
#define LED_BUILTIN 25
#endif


//------------------------------------------
// Default log interval in milli secs
#define kDefaultLogInterval 6000

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
spFormatJSON<1000> fmtJSON;
spFormatCSV fmtCSV;

spLogger  logger;

// Enable a timer with a default timer value - this is the log interval
spTimer   timer(kDefaultLogInterval);    // Timer 

// SD Card Filesystem object
spFSSDCard theSDCard;

// A writer interface for the SD Card that also rotates files 
spFileRotate  theOutputFile;

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

#ifdef OPENLOG_ESP32    
    pinMode(EN_3V3_SW, OUTPUT); // Enable Qwiic power and I2C
    digitalWrite(EN_3V3_SW, HIGH);
#endif
    // Start Spark - Init system: auto detects devices and restores settings from EEPROM
    //               This should be done after all devices are added..for now...
    spark.start();  

    // Logging is done at an interval - using an interval timer. 
    // Connect logger to the timer event
    logger.listen(timer.on_interval);  

    // We want to output JSON and CSV to the serial consol.
    //  - Add Serial to our  formatters
    fmtJSON.add(spSerial());
    fmtCSV.add(spSerial());    

    //  - Add the JSON and CVS format to the logger
    logger.add(fmtJSON);
    logger.add(fmtCSV);    

    // setup output to the SD card 
    if (theSDCard.initialize())
    {
        // SD card is available - lets setup output for it
        // Add the filesystem to the file output/rotation object
        theOutputFile.setFileSystem(theSDCard);

        // setup our file rotation parameters
        theOutputFile.filePrefix = "sfe";
        theOutputFile.startNumber=1;
        theOutputFile.rotatePeriod(24); // one day 

        // add the fileoutput to the CSV output.
        fmtCSV.add(theOutputFile);

        Serial.println("SD card output enabled");
    }
    else
        Serial.println("SD card output not available");

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
    // getAll() testing
    auto allButtons = spark.get<spDevButton>();

    Serial.printf("Number of buttons: %d \n\r", allButtons->size());
    for( auto button: *allButtons)
    {
        Serial.printf("Button Name: %s", button->name());

        // Have the button trigger a log entry
        logger.listen(button->on_clicked);
        
        // Lets long the value of the button event
        logger.listenLogEvent(button->on_clicked, button);        
    }

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
    // Just call the spark framework loop() method. Spark will manage
    // the dispatch of processing to the components that were added 
    // to the system during setup.
    if(spark.loop())        // will return true if an action did something
        digitalWrite(LED_BUILTIN, HIGH); 

    // Our loop delay 
    delay(1000); 
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

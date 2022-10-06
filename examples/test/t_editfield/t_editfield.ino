


#include <string>

#include "Spark.h"
#include "Spark/spSerialField.h"

spSerialField myField;


// For the test, two options:
//
// For strings, define STRING_TEST -- so output is done correctly 
//

//int8_t   Value=0;
//int32_t  Value=0;

//uint8_t  Value=0;
//uint32_t Value=0;

//float    Value=0.;
//double   Value=0.;

#define STRING_TEST
std::string Value="";

void setup() {

    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("\n---- Text Field Entry Test ----");
    Serial.println("\nCommands:");    
    Serial.println("\t- Delete/Backspace\tDelete previous character");       
    Serial.println("\t- Keypad DEL\t\tDelete next character");           
    Serial.println("\t- Arrow Left\t\tMove cursor to left");    
    Serial.println("\t- Arrow Right\t\tMove cursor to right");
    Serial.println("\t- Control-A\t\tMove to start of line");    
    Serial.println("\t- Control-E\t\tMove to end of line");
    Serial.println("\t- Control-K\t\tDelete text from cursor to end of line");
    Serial.println("\t- ESC\t\t\tStop Editing - discard changes");
    Serial.println("\t- Enter\t\t\tStop Editing - accept changes");    
    Serial.println();    

}


void loop() {


    Serial.print(">>");

    bool returnValue = myField.editField(Value);

    Serial.println();
    Serial.print("<<");

    if (returnValue)
#ifdef STRING_TEST
        Serial.println(Value.c_str());
#else
        Serial.println(Value);        
#endif
    else 
        Serial.println(" [ESC]");

    delay(100);
}

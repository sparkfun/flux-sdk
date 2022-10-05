


#include "Spark.h"
#include "Spark/spSerialField.h"

spSerialField myField;

char szBuffer[100] = {'\0'};

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
    //memset(szBuffer, '\0', sizeof(szBuffer));
    bool returnValue = myField.editField(szBuffer, sizeof(szBuffer));

    Serial.println();
    Serial.print("<<");

    if (returnValue)
        Serial.println(szBuffer);
    else 
        Serial.println(" [ESC]");

    delay(100);
}

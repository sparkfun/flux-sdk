


#include "Spark.h"
#include "Spark/spSerialField.h"

spSerialField myField;

char strValue[100] = {'\0'};

int8_t  int8Value=0;
int32_t  intValue=0;

uint8_t  uint8Value=0;
uint32_t uintValue=0;

float    fValue=0.;
double   dValue=0.;


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

    bool returnValue = myField.editField(strValue, sizeof(strValue));
    //bool returnValue = myField.editFieldInt(intValue);
    //bool returnValue = myField.editFieldInt8(int8Value);    
    //bool returnValue = myField.editFieldUInt(uintValue);
    //bool returnValue = myField.editFieldUInt8(uint8Value);    
    //bool returnValue = myField.editFieldFloat(fValue);        
    //bool returnValue = myField.editFieldDouble(dValue);            

    Serial.println();
    Serial.print("<<");

    if (returnValue)
        Serial.println(strValue);
    else 
        Serial.println(" [ESC]");

    delay(100);
}

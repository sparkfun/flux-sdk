/*
 * Spark Framework - Testing
 *
 * Test save restore of object properties
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>
#include "Spark/spStorageESP32Pref.h"

#include <math.h>

// Define a class that uses the properties of the system

class test_properties : public spAction
{

    bool    _b_data=false;
    int8_t  _i8_data = 0;
    int     _i_data=0;
    uint8_t _ui8_data=0;
    uint32_t _ui_data=0;
    float  _f_data=0.; 
    double _d_data=0;   
    std::string _s_data="";

public:

    test_properties(){

        spRegister(prop_bool, "bool_prop", "Testting bool property");
        spRegister(prop_int8, "int8_prop", "Testing int8 property");        
        spRegister(prop_int, "int32_prop", "Testing int32 property");
        spRegister(prop_uint8, "uint8_prop", "Testing uint8 property");        
        spRegister(prop_uint, "uint32_prop", "Testing uint32 property");        
        spRegister(prop_float, "float_prop", "Testing Float property");
        spRegister(prop_double, "double_prop", "Testing a double property");        
        spRegister(prop_str, "string_prop", "Testing a StringProperty");        

        spRegister(rw_prop_bool, "rw_bool", "Testing RW bool property");
        spRegister(rw_prop_int8, "rw_int8", "Testing RW integer8 property");
        spRegister(rw_prop_int, "rw_int", "Testing RW integer property");        
        spRegister(rw_prop_uint8, "rw_uint8", "Testing RW uinteger8 property");
        spRegister(rw_prop_uint, "rw_uint", "Testing RW uinteger property");                
        spRegister(rw_prop_float, "rw_float", "Testing RW float property");         
        spRegister(rw_prop_double, "rw_double", "Testing RW double property");        
        spRegister(rw_prop_str, "rw_string", "Testing RW string property");        

        setName("Test Properties");
    }
    


    // boolean setter/getter
    bool get_bool(void){

        return _b_data;
    }
    void set_bool( bool b){

        _b_data=b;

    };

    // int8 setter/getter
    int8_t get_int8(void){

        return _i8_data;
    }
    void set_int8( int8_t data){

        _i8_data=data;

    };
    // int setter/getter
    int get_int(void){

        return _i_data;
    }
    void set_int( int data){

        _i_data=data;

    };

    // uint8 setter/getter
    uint8_t get_uint8(void){

        return _ui8_data;
    }
    void set_uint8( uint8_t data){

        _ui8_data=data;

    };
    // uint setter/getter
    uint get_uint(void){

        return _ui_data;
    }
    void set_uint( uint data){

        _ui_data=data;

    };
    // float setter/getter
    float get_float(void){
        return _f_data;
    }
    void set_float( float data){
        _f_data=data;
    };

    // double setter/getter
    double get_double(void){
        return _d_data;
    }
    void set_double( double data){
        _d_data=data;
    };

    // int setter/getter
    std::string get_str(void){

        return _s_data;
    }
    void set_str(std::string data){

        _s_data=data;

    };
    // Define standard properties 
    spPropertyBool<test_properties>     prop_bool;
    spPropertyInt8<test_properties>     prop_int8;
    spPropertyInt<test_properties>      prop_int;    
    spPropertyUint8<test_properties>    prop_uint8;
    spPropertyUint<test_properties>     prop_uint;        
    spPropertyFloat<test_properties>    prop_float;
    spPropertyDouble<test_properties>    prop_double;    
    spPropertyString<test_properties>   prop_str;

    // Define RW (getter/setter) Properties
    spPropertyRWBool<test_properties, &test_properties::get_bool, &test_properties::set_bool> rw_prop_bool;

    spPropertyRWInt8<test_properties, &test_properties::get_int8, &test_properties::set_int8> rw_prop_int8;    
    spPropertyRWInt<test_properties, &test_properties::get_int, &test_properties::set_int> rw_prop_int;        

    spPropertyRWUint8<test_properties, &test_properties::get_uint8, &test_properties::set_uint8> rw_prop_uint8;    
    spPropertyRWUint<test_properties, &test_properties::get_uint, &test_properties::set_uint> rw_prop_uint;        
    
    spPropertyRWFloat<test_properties, &test_properties::get_float, &test_properties::set_float> rw_prop_float;        
    spPropertyRWDouble<test_properties, &test_properties::get_double, &test_properties::set_double> rw_prop_double;        

    spPropertyRWString<test_properties, &test_properties::get_str, &test_properties::set_str> rw_prop_str;    
};


// Define testing values

#define kTest0_Bool     false
#define kTest0_Int8     -21
#define kTest0_Int      1434
#define kTest0_Uint8    222
#define kTest0_Uint     3212
#define kTest0_Float    3.141592
#define kTest0_Double   925.1313
#define kTest0_String  "This is a STRING"

#define kTest1_Bool     true
#define kTest1_Int8     -33
#define kTest1_Int      32111
#define kTest1_Uint8    12
#define kTest1_Uint     4521
#define kTest1_Float    32.3289
#define kTest1_Double   99.9981
#define kTest1_String  "a different string"


/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// Uses parameter introspection to print output values of the BME280
// 
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

test_properties myTest; 

void test_setup()
{

    myTest.prop_bool = kTest0_Bool;
    myTest.prop_int8 = kTest0_Int8;
    myTest.prop_int = kTest0_Int;
    myTest.prop_uint8 = kTest0_Uint8;
    myTest.prop_uint = kTest0_Uint;
    myTest.prop_float = kTest0_Float;
    myTest.prop_double = kTest0_Double;
    myTest.prop_str = kTest0_String;

    myTest.rw_prop_bool = kTest1_Bool;
    myTest.rw_prop_int8 = kTest1_Int8;
    myTest.rw_prop_int = kTest1_Int;
    myTest.rw_prop_uint8 = kTest1_Uint8;
    myTest.rw_prop_uint = kTest1_Uint;
    myTest.rw_prop_float = kTest1_Float;
    myTest.rw_prop_double = kTest1_Double;
    myTest.rw_prop_str = kTest1_String;

}

void test_change()
{

    myTest.prop_bool = kTest1_Bool;
    myTest.prop_int8 = kTest1_Int8;
    myTest.prop_int = kTest1_Int;
    myTest.prop_uint8 = kTest1_Uint8;
    myTest.prop_uint = kTest1_Uint;
    myTest.prop_float = kTest1_Float;
    myTest.prop_double = kTest1_Double;
    myTest.prop_str = kTest1_String;

    myTest.rw_prop_bool = kTest0_Bool;
    myTest.rw_prop_int8 = kTest0_Int8;
    myTest.rw_prop_int = kTest0_Int;
    myTest.rw_prop_uint8 = kTest0_Uint8;
    myTest.rw_prop_uint = kTest0_Uint;
    myTest.rw_prop_float = kTest0_Float;
    myTest.rw_prop_double = kTest0_Double;
    myTest.rw_prop_str = kTest0_String;

}
void test_check()
{

    if (myTest.prop_bool != kTest0_Bool )
        Serial.printf("\tError - property type bool restore fail\n\r");
    else
        Serial.printf("\tProperty type bool restore success\n\r");

    if (myTest.prop_int8 != kTest0_Int8)
        Serial.printf("\tError - property type int8 restore fail\n\r");
    else
        Serial.printf("\tProperty type int8 restore success\n\r");

    if (myTest.prop_int != kTest0_Int )
        Serial.printf("\tError - property type int restore fail\n\r");
    else
        Serial.printf("\tProperty type int restore success\n\r");

    if (myTest.prop_uint8 != kTest0_Uint8 )
        Serial.printf("\tError - property type uint8 restore fail\n\r");
    else
        Serial.printf("\tProperty type uint8 restore success\n\r");

    if (myTest.prop_uint != kTest0_Uint)
        Serial.printf("\tError - property type uint restore fail\n\r");
    else
        Serial.printf("\tProperty type uint restore success\n\r");

    // use abs() to deal with floating point math 
    if ( fabsf(myTest.prop_float - kTest0_Float) > .001 )
        Serial.printf("\tError - property type float restore fail\n\r");
    else
        Serial.printf("\tProperty type float restore success\n\r");

    // use abs() to deal with floating point math 
    if ( fabs(myTest.prop_double - kTest0_Double) > .0001 )
        Serial.printf("\tError - property type double restore fail\n\r");
    else
        Serial.printf("\tProperty type double restore success\n\r");

    if (myTest.prop_str != kTest0_String )
        Serial.printf("\tError - property type string restore fail\n\r");
    else
        Serial.printf("\tProperty type string restore success\n\r");    


    if (myTest.rw_prop_bool != kTest1_Bool )
        Serial.printf("\tError - RW property type bool restore fail\n\r");
    else
        Serial.printf("\tRW Property type bool restore success\n\r");

    if (myTest.rw_prop_int8 != kTest1_Int8)
        Serial.printf("\tError - RW property type int8 restore fail\n\r");
    else
        Serial.printf("\tRW Property type int8 restore success\n\r");

    if (myTest.rw_prop_int != kTest1_Int )
        Serial.printf("\tError - RW property type int restore fail\n\r");
    else
        Serial.printf("\tRW Property type int restore success\n\r");

    if (myTest.rw_prop_uint8 != kTest1_Uint8 )
        Serial.printf("\tError - RW property type uint8 restore fail\n\r");
    else
        Serial.printf("\tRW Property type uint8 restore success\n\r");

    if (myTest.rw_prop_uint != kTest1_Uint)
        Serial.printf("\tError - RW property type uint restore fail\n\r");
    else
        Serial.printf("\tRW Property type uint restore success\n\r");

    // use abs() to deal with floating point math 
    if ( fabsf(myTest.rw_prop_float - kTest1_Float) > .001 )
        Serial.printf("\tError - RW property type float restore fail\n\r");
    else
        Serial.printf("\tRW Property type float restore success\n\r");

    // use abs() to deal with floating point math 
    if (fabs(myTest.rw_prop_double -kTest1_Double) > .0001 )
        Serial.printf("\tError - RW property type double restore fail\n\r");
    else
        Serial.printf("\tRW Property type double restore success\n\r");

    if (myTest.rw_prop_str != kTest1_String )
        Serial.printf("\tError - RW property type string restore fail\n\r");
    else
        Serial.printf("\tRW Property type string restore success\n\r");    
    

}

spStorageESP32Pref myStorage;

//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup of the Save/Restore tests ----");

    spark.add(myTest);

    Serial.println("object Added, reseting storage");
    myStorage.resetStorage();

    test_setup();
    Serial.println("Saving...");    
    spark.save(&myStorage);

    Serial.println("Save complete");

    test_change();

    Serial.printf("\n\rNew values in place - verify - everything should signal an ERROR\n\r\n\r");

    test_check();

    spark.restore(&myStorage);

    Serial.println("\n\rValues restored - verify - everything should pass\n\r");    

    test_check();
    
    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    // Retrieve the data from the devices.
    digitalWrite(LED_BUILTIN, HIGH);   // turn on the log led    
    
    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

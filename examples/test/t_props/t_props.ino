/*
 * Spark Framework - Testing
 *
 * Test properties
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>


// Define a class that uses the properties of the system

class test_properties : public spObject
{

    bool   _b_data=false;
    int    _i_data=0;

public:

    test_properties(){

        prop_bool(this);
        prop_int(this);
        prop_float(this);

        rw_prop_bool(this);
        rw_prop_int(this);
    }
    


    // boolean setter/getter
    bool get_bool(void){

        return _b_data;
    }
    void set_bool(const bool &b){

        _b_data=b;

    };

    // int setter/getter
    int get_int(void){

        return _i_data;
    }
    void set_int(const int &data){

        _i_data=data;

    };
    // Define standard properties 
    spPropertyBool2<test_properties>     prop_bool;
    spPropertyInt2<test_properties>      prop_int;
    spPropertyFloat2<test_properties>    prop_float;
    //spPropertyString2<test_properties>   prop_string;

    // Define RW (getter/setter) Properties
    spPropertyRWBool<test_properties, &test_properties::get_bool, &test_properties::set_bool> rw_prop_bool;
    spPropertyRWInt<test_properties, &test_properties::get_int, &test_properties::set_int> rw_prop_int;    
};


/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// Uses parameter instrospection to print output values of the BME280
// 
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

test_properties myTest; 

void run_tests()
{

    Serial.println("BOOL Tests:");
    bool b_test = false;

    myTest.prop_bool = b_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.prop_bool == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.prop_bool = b_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.prop_bool == b_test ? "PASS" : "FAIL"));    

    b_test = false;
    myTest.prop_bool(b_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_bool() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.prop_bool(b_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_bool() == b_test ? "PASS" : "FAIL"));  

    b_test = false;
    myTest.prop_bool.set(b_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_bool.get() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.prop_bool.set(b_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_bool.get() == b_test ? "PASS" : "FAIL"));


    Serial.println("Int Tests:");
    int i_test = 3;

    myTest.prop_int = i_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.prop_int == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.prop_int = i_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.prop_int == i_test ? "PASS" : "FAIL"));    

    i_test++;
    myTest.prop_int(i_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_int() == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.prop_int(i_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_int() == i_test ? "PASS" : "FAIL"));  

    i_test++;
    myTest.prop_int.set(i_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_int.get() == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.prop_int.set(i_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_int.get() == i_test ? "PASS" : "FAIL"));
}


void run_rw_tests()
{

    Serial.println("BOOL RW Tests:");
    bool b_test = false;

    myTest.rw_prop_bool = b_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.rw_prop_bool == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.rw_prop_bool = b_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.rw_prop_bool == b_test ? "PASS" : "FAIL"));    

    b_test = false;
    myTest.rw_prop_bool(b_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_bool() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.rw_prop_bool(b_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_bool() == b_test ? "PASS" : "FAIL"));  

    b_test = false;
    myTest.rw_prop_bool.set(b_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_bool.get() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.rw_prop_bool.set(b_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_bool.get() == b_test ? "PASS" : "FAIL"));


    Serial.println("Int RW Tests:");
    int i_test = 3;

    myTest.rw_prop_int = i_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.rw_prop_int == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.rw_prop_int = i_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.rw_prop_int == i_test ? "PASS" : "FAIL"));    

    i_test++;
    myTest.rw_prop_int(i_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_int() == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.rw_prop_int(i_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_int() == i_test ? "PASS" : "FAIL"));  

    i_test++;
    myTest.rw_prop_int.set(i_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_int.get() == i_test ? "PASS" : "FAIL"));

    i_test++;
    myTest.rw_prop_int.set(i_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_int.get() == i_test ? "PASS" : "FAIL"));
}
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

    run_tests();
    run_rw_tests();
    
    digitalWrite(LED_BUILTIN, LOW);  // board LED off
}

//---------------------------------------------------------------------
// Arduino loop - 
void loop() {

    // Retrieve the data from the devices.
    digitalWrite(LED_BUILTIN, HIGH);   // turn on the log led    
    
    Serial.println();
    // Our loop delay 
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    delay(1000);
}

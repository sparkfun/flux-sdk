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
    float  _f_data=0.;    
    std::string _s_data="";

public:

    test_properties(){

        spRegisterProperty2(prop_bool);
        spRegisterProperty2(prop_int, "MyInteger", "Testing integer property");
        spRegisterProperty2(prop_float, "FloatValue");
        spRegisterProperty2(prop_str, "stringProp", "Testing a StringProperty");        

        spRegisterProperty2(rw_prop_bool);
        spRegisterProperty2(rw_prop_int, "rw_int", "Testing Read/Write integer property");
        spRegisterProperty2(rw_prop_str);
        spRegisterProperty2(rw_prop_float);
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

    // float setter/getter
    float get_float(void){

        return _f_data;
    }
    void set_float(const float &data){

        _f_data=data;
    };

    // int setter/getter
    std::string get_str(void){

        return _s_data;
    }
    void set_str(const std::string &data){

        _s_data=data;

    };
    // Define standard properties 
    spPropertyBool2<test_properties>     prop_bool;
    spPropertyInt2<test_properties>      prop_int;
    spPropertyFloat2<test_properties>    prop_float;
    spPropertyString2<test_properties>   prop_str;

    // Define RW (getter/setter) Properties
    spPropertyRWBool<test_properties, &test_properties::get_bool, &test_properties::set_bool> rw_prop_bool;
    spPropertyRWInt<test_properties, &test_properties::get_int, &test_properties::set_int> rw_prop_int;    
    spPropertyRWFloat<test_properties, &test_properties::get_float, &test_properties::set_float> rw_prop_float;        
    spPropertyRWString<test_properties, &test_properties::get_str, &test_properties::set_str> rw_prop_str;    
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

    Serial.print("NAME: "); Serial.print(myTest.prop_bool.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.prop_bool.description.c_str());
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

    //---------------------------------------------------------------------------------------------------
    Serial.println("Int Tests:");
    Serial.print("NAME: "); Serial.print(myTest.prop_int.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.prop_int.description.c_str());
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

//---------------------------------------------------------------------------------------------------
    Serial.println("Float RW Tests:");
    float f_test = 3;
    Serial.print("NAME: "); Serial.print(myTest.prop_float.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.prop_float.description.c_str());

    myTest.prop_float = f_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.prop_float == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.prop_float = f_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.prop_float == f_test ? "PASS" : "FAIL"));    

    f_test++;
    myTest.prop_float(f_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_float() == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.prop_float(f_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_float() == f_test ? "PASS" : "FAIL"));  

    f_test++;
    myTest.prop_float.set(f_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_float.get() == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.prop_float.set(f_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_float.get() == f_test ? "PASS" : "FAIL"));

    //---------------------------------------------------------------------------------------------------
    Serial.println("String Tests:");
    std::string s_test = "one";

    Serial.print("NAME: "); Serial.print(myTest.prop_str.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.prop_str.description.c_str());

    myTest.prop_str = s_test;

    Serial.print("   Test 1: "); Serial.println( (myTest.prop_str == s_test ? "PASS" : "FAIL"));

    s_test = "two";
    myTest.prop_str = s_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.prop_str == s_test ? "PASS" : "FAIL"));    

    s_test = "three";
    myTest.prop_str(s_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_str() == s_test ? "PASS" : "FAIL"));

    s_test = "four";
    myTest.prop_str(s_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_str() == s_test ? "PASS" : "FAIL"));  

    s_test = "five";
    myTest.prop_str.set(s_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.prop_str.get() == s_test ? "PASS" : "FAIL"));

    s_test = "six";
    myTest.prop_str.set(s_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.prop_str.get() == s_test ? "PASS" : "FAIL"));
}


void run_rw_tests()
{

    Serial.println("BOOL RW Tests:");
    bool b_test = false;

    Serial.print("NAME: "); Serial.print(myTest.rw_prop_bool.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.rw_prop_bool.description.c_str());
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


    //---------------------------------------------------------------------------------------------------
    Serial.println("Int RW Tests:");
    int i_test = 3;

    Serial.print("NAME: "); Serial.print(myTest.rw_prop_int.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.rw_prop_int.description.c_str());

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

    //---------------------------------------------------------------------------------------------------
    Serial.println("Float Tests:");
    float f_test = 3;

    Serial.print("NAME: "); Serial.print(myTest.rw_prop_float.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.rw_prop_float.description.c_str());

    myTest.rw_prop_float = f_test;
    Serial.print("   Test 1: "); Serial.println( (myTest.rw_prop_float == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.rw_prop_float = f_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.rw_prop_float == f_test ? "PASS" : "FAIL"));    

    f_test++;
    myTest.rw_prop_float(f_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_float() == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.rw_prop_float(f_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_float() == f_test ? "PASS" : "FAIL"));  

    f_test++;
    myTest.rw_prop_float.set(f_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_float.get() == f_test ? "PASS" : "FAIL"));

    f_test++;
    myTest.rw_prop_float.set(f_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_float.get() == f_test ? "PASS" : "FAIL"));


    //---------------------------------------------------------------------------------------------------
    Serial.println("String RW Tests:");
    std::string s_test = "one";

    Serial.print("NAME: "); Serial.print(myTest.rw_prop_str.name2.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.rw_prop_str.description.c_str());

    myTest.rw_prop_str = s_test;

    Serial.print("   Test 1: "); Serial.println( (myTest.rw_prop_str == s_test ? "PASS" : "FAIL"));

    s_test = "two";
    myTest.rw_prop_str = s_test;
    Serial.print("   Test 2: "); Serial.println( (myTest.rw_prop_str == s_test ? "PASS" : "FAIL"));    

    s_test = "three";
    myTest.rw_prop_str(s_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_str() == s_test ? "PASS" : "FAIL"));

    s_test = "four";
    myTest.rw_prop_str(s_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_str() == s_test ? "PASS" : "FAIL"));  

    s_test = "five";
    myTest.rw_prop_str.set(s_test);
    Serial.print("   Test 3: "); Serial.println( (myTest.rw_prop_str.get() == s_test ? "PASS" : "FAIL"));

    s_test = "six";
    myTest.rw_prop_str.set(s_test);
    Serial.print("   Test 4: "); Serial.println( (myTest.rw_prop_str.get() == s_test ? "PASS" : "FAIL"));
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

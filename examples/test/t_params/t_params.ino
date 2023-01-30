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
 * Spark Framework - Testing
 *
 * Test properties
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Flux.h>


// Define a class that uses the parameters of the system

class test_params : public flxOperation
{

    bool            _b_data=false;
    int             _i_data=0;
    int8_t          _i8_data=0;    
    int16_t         _i16_data=0;
    uint8_t          _ui8_data=0;        
    uint16_t        _ui16_data=0;    
    float           _f_data=0.;    
    std::string     _s_data="";

public:

    test_params(){

        flxRegister(out_bool);
        flxRegister(out_int, "MyInteger", "Testing Int output parameter");
        flxRegister(out_int8, "Int8 Out", "Testing Int8 output parameter");        
        flxRegister(out_int16, "Int16 Out", "Testing Int16 output parameter");                
        flxRegister(out_uint8, "UInt8 Out", "Testing UInt8 output parameter");                
        flxRegister(out_uint16, "UInt16 Out", "Testing UInt16 output parameter");                        
        flxRegister(out_float, "Float Out");
        flxRegister(out_string, "Out String", "Testing a String Output Parameter");        

        flxRegister(in_bool, "BOOL IN");
        flxRegister(in_int8, "Int8 In");        
        flxRegister(in_int16, "Int16 In");            
        flxRegister(in_int, "InInt", "Testing Integer Input Parameter");
        flxRegister(in_uint8, "UInt8 In");                
        flxRegister(in_uint16, "UInt16 In");                        
        flxRegister(in_float);
        flxRegister(in_string);

        flxRegister(out_int_arr, "Output Int Array", "Testing array output");

    }
    


    // boolean setter/getter
    bool get_bool(void);
    void set_bool(const bool &b);

    // int8 setter/getter
    int8_t get_int8(void);
    void set_int8(const int8_t &);

    // int16 setter/getter
    int16_t get_int16(void);
    void set_int16(const int16_t &);

    // int setter/getter
    int get_int(void);
    void set_int(const int &);

    // uint8 setter/getter
    uint8_t get_uint8(void);
    void set_uint8(const uint8_t &);

    // uint16 setter/getter
    uint16_t get_uint16(void);
    void set_uint16(const uint16_t &);

    // float setter/getter
    float get_float(void);
    void set_float(const float &);

    // string setter/getter
    std::string get_str(void);
    void set_str(const std::string &);


    // out arrays
    bool get_int_arr(flxDataArrayInt *);

    // Output Parameters
    flxParameterOutBool<test_params, &test_params::get_bool>         out_bool;
    flxParameterOutInt8<test_params, &test_params::get_int8>         out_int8;    
    flxParameterOutInt16<test_params, &test_params::get_int16>       out_int16;
    flxParameterOutInt<test_params, &test_params::get_int>           out_int;
    flxParameterOutUint8<test_params, &test_params::get_uint8>       out_uint8;        
    flxParameterOutUint16<test_params, &test_params::get_uint16>     out_uint16;        
    flxParameterOutFloat<test_params, &test_params::get_float>       out_float;
    flxParameterOutString<test_params, &test_params::get_str>        out_string;

    flxParameterInBool<test_params, &test_params::set_bool>          in_bool;
    flxParameterInInt8<test_params, &test_params::set_int8>        in_int8;
    flxParameterInInt16<test_params, &test_params::set_int16>        in_int16;    
    flxParameterInInt<test_params, &test_params::set_int>            in_int;
    flxParameterInUint8<test_params, &test_params::set_uint8>      in_uint8;        
    flxParameterInUint16<test_params, &test_params::set_uint16>      in_uint16;            
    flxParameterInFloat<test_params, &test_params::set_float>        in_float;
    flxParameterInString<test_params, &test_params::set_str>         in_string;    


    flxParameterOutArrayInt<test_params, &test_params::get_int_arr>  out_int_arr;
};

// Method implementation

// boolean setter/getter
bool test_params::get_bool(void){
    return _b_data;
}
void test_params::set_bool(const bool &b){
        _b_data=b;
};

// int8 setter/getter
int8_t test_params::get_int8(void){
    return _i8_data;
}
void test_params::set_int8(const int8_t &data){
    _i8_data=data;
};

// int16 setter/getter
int16_t test_params::get_int16(void){
    return _i16_data;
}
void test_params::set_int16(const int16_t &data){
    _i16_data=data;
};

// int setter/getter
int test_params::get_int(void){
    return _i_data;
}
void test_params::set_int(const int &data){
    _i_data=data;
};

// uint8 setter/getter
uint8_t test_params::get_uint8(void){
    return _ui8_data;
}
void test_params::set_uint8(const uint8_t &data){
    _ui8_data=data;
};

// uint16 setter/getter
uint16_t test_params::get_uint16(void){
    return _ui16_data;
}
void test_params::set_uint16(const uint16_t &data){
    _ui16_data=data;
};

// float setter/getter
float test_params::get_float(void){
    return _f_data;
}
void test_params::set_float(const float &data){
    _f_data=data;
};

// str setter/getter
std::string test_params::get_str(void){
        return _s_data;
}
void test_params::set_str(const std::string &data){
        _s_data=data;    
}

// Array

// out arrays
bool test_params::get_int_arr(flxDataArrayInt *theArray)
{
    static int mydata[] = {1, 2, 3, 4, 5, 6};

    theArray->set(mydata, sizeof(mydata)/sizeof(int), false);

    return true;
}
/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// Uses parameter introspection to print output values of the BME280
// 
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

test_params myTest; 

void run_tests()
{

    Serial.println("BOOL Tests:");
    bool b_test = false;

    Serial.print("Input NAME: "); Serial.print(myTest.in_bool.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_bool.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_bool.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_bool.description());  

    myTest.in_bool(b_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_bool() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.in_bool.set(b_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_bool.get() == b_test ? "PASS" : "FAIL"));    

    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("Int8 Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_int8.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_int8.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_int8.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_int8.description());

    int8_t i8_test = -23;

    myTest.in_int8(i8_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_int8() == i8_test ? "PASS" : "FAIL"));

    i8_test+=25;
    myTest.in_int8.set(i8_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_int8.get() == i8_test ? "PASS" : "FAIL"));    


    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("Int16 Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_int16.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_int16.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_int16.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_int16.description());

    int16_t i16_test = -13;

    myTest.in_int16(i16_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_int16() == i16_test ? "PASS" : "FAIL"));

    i16_test+=5;
    myTest.in_int16.set(i16_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_int16.get() == i16_test ? "PASS" : "FAIL"));  

    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("Int Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_int.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_int.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_int.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_int.description());

    int i_test = 3;

    myTest.in_int(i_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_int() == i_test ? "PASS" : "FAIL"));

    i_test+=5;
    myTest.in_int.set(i_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_int.get() == i_test ? "PASS" : "FAIL"));  


    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("UInt8 Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_uint8.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_uint8.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_uint8.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_uint8.description());

    uint8_t ui8_test = 43;

    myTest.in_uint8(ui8_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_uint8() == ui8_test ? "PASS" : "FAIL"));

    ui8_test+=26;
    myTest.in_uint8.set(ui8_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_uint8.get() == ui8_test ? "PASS" : "FAIL")); 

    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("UInt16 Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_uint16.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_uint16.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_uint16.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_uint16.description());

    uint16_t ui16_test = -13;

    myTest.in_uint16(ui16_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_uint16() == ui16_test ? "PASS" : "FAIL"));

    ui16_test+=56;
    myTest.in_uint16.set(ui16_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_uint16.get() == ui16_test ? "PASS" : "FAIL")); 



    //---------------------------------------------------------------------------------------------------
    Serial.println();    
    Serial.println("Float Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_float.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_float.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_float.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_float.description());

    float f_test = 3.134;

    myTest.in_float(f_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_float() == f_test ? "PASS" : "FAIL"));
    f_test+=5.;
    myTest.in_float.set(f_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_float.get() == f_test ? "PASS" : "FAIL"));   

    //---------------------------------------------------------------------------------------------------
    Serial.println();    
    Serial.println("String Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_string.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_string.description());
    Serial.print("Output NAME: "); Serial.print(myTest.out_string.name()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_string.description());

    std::string s_test = "COW";

    myTest.in_string(s_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_string() == s_test ? "PASS" : "FAIL"));
    s_test+="goes moo";
    myTest.in_string.set(s_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_string.get() == s_test ? "PASS" : "FAIL"));  

    Serial.println();    
    Serial.println("DONE"); 


    // Array tests

    Serial.println("Int Array Test:");
    flxDataArrayInt *iArr = myTest.out_int_arr.get();
    if (!iArr)
        Serial.println("Error accessing int array");
    else
    {
        int * pData = iArr->get();
        Serial.print("Array Data: [");
        for (int i=0; i < iArr->size(); i++)
            Serial.printf( "%s%d", (i > 0 ? ", " : ""), *pData++);
        Serial.println("]");

        delete iArr;
    }

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

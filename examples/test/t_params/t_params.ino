/*
 * Spark Framework - Testing
 *
 * Test properties
 *   
 */

#include "Arduino.h"

// Spark framework 
#include <Spark.h>


// Define a class that uses the parameters of the system

class test_params : public spOperation
{

    bool   _b_data=false;
    int    _i_data=0;
    float  _f_data=0.;    
    std::string _s_data="";

public:

    test_params(){

        spRegister(out_bool);
        spRegister(out_int, "MyInteger", "Testing Int output parameter");
        spRegister(out_float, "Float Out");
        spRegister(out_string, "Out String", "Testing a String Output Parameter");        

        spRegister(in_bool);
        spRegister(in_int, "InInt", "Testing Integer Input Parameter");
        spRegister(in_string);
        spRegister(in_float);
    }
    


    // boolean setter/getter
    bool get_bool(void);
    void set_bool(const bool &b);

    // int setter/getter
    int get_int(void);
    void set_int(const int &);

    // float setter/getter
    float get_float(void);
    void set_float(const float &);

    // string setter/getter
    std::string get_str(void);
    void set_str(const std::string &);


    // Output Parameters
    spParameterOutBool<test_params, &test_params::get_bool>   out_bool;
    spParameterOutInt<test_params, &test_params::get_int>     out_int;
    spParameterOutFloat<test_params, &test_params::get_float> out_float;
    spParameterOutString<test_params, &test_params::get_str>  out_string;

    spParameterInBool<test_params, &test_params::set_bool>   in_bool;
    spParameterInInt<test_params, &test_params::set_int>     in_int;
    spParameterInFloat<test_params, &test_params::set_float> in_float;
    spParameterInString<test_params, &test_params::set_str>  in_string;     
};

// Method implementation

// boolean setter/getter
bool test_params::get_bool(void){
    return _b_data;
}
void test_params::set_bool(const bool &b){
        _b_data=b;
};

// int setter/getter
int test_params::get_int(void){
    return _i_data;
}
void test_params::set_int(const int &data){
    _i_data=data;
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
/////////////////////////////////////////////////////////////////////////
//
// Simple demo - connect to devices directly.
//
// Uses parameter instrospection to print output values of the BME280
// 
// For this demo, connect to a BME280 and a CCS811 (the env combo board)

test_params myTest; 

void run_tests()
{

    Serial.println("BOOL Tests:");
    bool b_test = false;

    Serial.print("Input NAME: "); Serial.print(myTest.in_bool.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_bool.description.c_str());
    Serial.print("Output NAME: "); Serial.print(myTest.out_bool.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_bool.description.c_str());  

    myTest.in_bool(b_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_bool() == b_test ? "PASS" : "FAIL"));

    b_test = true;
    myTest.in_bool.set(b_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_bool.get() == b_test ? "PASS" : "FAIL"));    

    //---------------------------------------------------------------------------------------------------
    Serial.println();
    Serial.println("Int Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_int.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_int.description.c_str());
    Serial.print("Output NAME: "); Serial.print(myTest.out_int.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_int.description.c_str());

    int i_test = 3;

    myTest.in_int(i_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_int() == i_test ? "PASS" : "FAIL"));

    i_test+=5;
    myTest.in_int.set(i_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_int.get() == i_test ? "PASS" : "FAIL"));    

    //---------------------------------------------------------------------------------------------------
    Serial.println();    
    Serial.println("Float Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_float.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_float.description.c_str());
    Serial.print("Output NAME: "); Serial.print(myTest.out_float.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_float.description.c_str());

    float f_test = 3.134;

    myTest.in_float(f_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_float() == f_test ? "PASS" : "FAIL"));
    f_test+=5.;
    myTest.in_float.set(f_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_float.get() == f_test ? "PASS" : "FAIL"));   

    //---------------------------------------------------------------------------------------------------
    Serial.println();    
    Serial.println("String Tests:");
    Serial.print("Input NAME: "); Serial.print(myTest.in_string.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.in_string.description.c_str());
    Serial.print("Output NAME: "); Serial.print(myTest.out_string.name.c_str()); 
        Serial.print("  DESC: "); Serial.println(myTest.out_string.description.c_str());

    std::string s_test = "COW";

    myTest.in_string(s_test);
    Serial.print("   Test 1: "); Serial.println( (myTest.out_string() == s_test ? "PASS" : "FAIL"));
    s_test+="goes moo";
    myTest.in_string.set(s_test);
    Serial.print("   Test 2: "); Serial.println( (myTest.out_string.get() == s_test ? "PASS" : "FAIL"));  

    Serial.println();    
    Serial.println("DONE");        
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

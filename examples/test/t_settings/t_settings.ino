/*
 * Spark Framework demo - logging
 *   
 */

// Spark framework 
#include <Spark.h>
#include <Spark/flxLogger.h>
#include <Spark/flxFmtJSON.h>
#include <Spark/flxFmtCSV.h>
#include <Spark/flxTimer.h>
#include <Spark/flxSerial.h>

#include <Spark/flxStorageESP32Pref.h>
#include <Spark/flxSettingsSerial.h>
#include <Spark/flxSettings.h>



class test_properties : public flxActionType<test_properties>
{

    bool   _b_data=false;
    int    _i_data=0;
    uint16_t _ui16_data=0;
    float  _f_data=0.;    
    std::string _s_data="";

public:

    test_properties(){

        setName("Test Properties", "Property testing object");

        spRegister(prop_bool);
        
        spRegister(prop_int, "MyInteger", "Testing integer property - No initial value, a set Limit set");
        spRegister(prop_int_set, "Int Set Test", "Testing integer property - with limit set in init");  


        spRegister(prop_uint_range, "Uint Range Test", "No initial value, range limit set");    
        // Change to a validation set:
        prop_uint_range.addDataLimitValidValue( {
                                            {"Value One", 22},
                                            {"Value Two", 44},
                                            {"Value Three", 66},        
                                            {"Value Four", 88},      
                                            {"Value Five", 110}                       
                                        });
        spRegister(prop_uint16, "Uint16 Test", "Uint 16, standard prop, initial value");
        //prop_uint16.setDataLimitRange(43, 98);

        spRegister(prop_float, "FloatValue", "Float with an initial value and a limit range");
        // change to a data limit set
        prop_float.addDataLimitValidValue("ONE K", 1000.);
        prop_float.addDataLimitValidValue("TWO K", 2000.);
        prop_float.addDataLimitValidValue("THREE K", 3000.);
        prop_float.addDataLimitValidValue("FOUR K", 4000.);


        //spRegister(prop_str, "stringProp", "Testing a StringProperty");        

        spRegister(rw_prop_bool);
        spRegister(rw_prop_float, "RW Float", "Float property with a range limit");                
        // Register the RW int property and add the data limit set.
        spRegister(rw_prop_int, "rw_int", "Testing Read/Write integer property with data limit set");        

        spRegister(rw_prop_uint16, "RW UInt16", "UInt 16 RW property, with an initial value");

        spRegister(rw_prop_str);
        

    }
    


    // boolean setter/getter
    bool get_bool(void){

        return _b_data;
    }
    void set_bool( bool b){

        _b_data=b;

    };

    // int setter/getter
    int get_int(void){

        return _i_data;
    }
    void set_int( int data){

        _i_data=data;

    };

    // int setter/getter
    uint16_t get_uint16(void){

        return _ui16_data;
    }
    void set_uint16( uint16_t data){

        _ui16_data=data;

    };

    // float setter/getter
    float get_float(void){

        return _f_data;
    }
    void set_float( float data){

        _f_data=data;
    };

    // int setter/getter
    std::string get_str(void){

        return _s_data;
    }
    void set_str(std::string data){

        _s_data=data;

    };
    // Define standard properties 
    flxPropertyBool<test_properties>     prop_bool;

    // int property, with no initial value, but a set of limited values
    flxPropertyUint8<test_properties>      prop_int={
                                            {"Value One", 10},
                                            {"Value Two", 22},
                                            {"Value Three", 33},        
                                            {"Value Four", 44},      
                                            {"Value Five", 155}                       
                                        };
    // Test property with an initial value and a value limit set.
    flxPropertyUint8<test_properties>      prop_int_set={44, {
                                            {"Value 1One", 10},
                                            {"Value 1Two", 22},
                                            {"Value 1Three", 33},        
                                            {"Value 1Four", 44},      
                                            {"Value 1Five", 155}                       
                                            }};
    flxPropertyUint<test_properties>       prop_uint_range = { 20, 60 };

    flxPropertyUint16<test_properties>     prop_uint16={12, 1, 13};

    // A float property with an initial value and a limit range
    flxPropertyFloat<test_properties>    prop_float = {1.0, -1.0, 20.0};
    //flxPropertyString<test_properties>   prop_str = {"starter string"};

    // Define RW (getter/setter) Properties
    flxPropertyRWBool<test_properties, &test_properties::get_bool, &test_properties::set_bool> rw_prop_bool;
    
    // Read-write property, with an initial value, and a set of Valid VAlues
    flxPropertyRWInt<test_properties, &test_properties::get_int, &test_properties::set_int> rw_prop_int = {111, {
                                                                                                    {"Value One", 111},
                                                                                                    {"Value Two", 222},
                                                                                                    {"Value Three", 333},        
                                                                                                    {"Value Four", 444},      
                                                                                                    {"Value Five", 555}                       
                                                                                                    } };

    // uint 16 w/ initial value
    flxPropertyRWUint16<test_properties, &test_properties::get_uint16, &test_properties::set_uint16> rw_prop_uint16 = {4444};    

    // Float with a initial value and a range limit
    flxPropertyRWFloat<test_properties, &test_properties::get_float, &test_properties::set_float> rw_prop_float = {93, -100, 100}; 
    
    flxPropertyRWString<test_properties, &test_properties::get_str, &test_properties::set_str> rw_prop_str = {"rw string initial value"};   



};

// Define an action class that uses parameters for testing

class test_params : public flxActionType<test_params>
{

    bool   _b_data=false;
    int    _i_data=0;
    float  _f_data=0.;    
    std::string _s_data="";

public:

    test_params(){

        setName("Parameter Test", "Used to test input and output parameters");

        spRegister(out_bool, "Output Bool", "Test an output bool parameter");
        spRegister(out_int, "MyInteger", "Testing Int output parameter");
        spRegister(out_float, "Float Out");
        spRegister(out_string, "Out String", "Testing a String Output Parameter");        

        spRegister(in_bool, "in bool", "test input of a bool value");
        
        spRegister(in_int,  "in int", "test input of a int value with a range limit");

        spRegister(in_string, "in string", "test input of a string value");
        spRegister(in_float, "in float", "test input of a float value");

        spRegister(in_void, "in void", "test input parameter of type void");
    };
    


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

    // VOID
    void set_void(void);


    // Output Parameters
    flxParameterOutBool<test_params, &test_params::get_bool>   out_bool;
    flxParameterOutInt<test_params, &test_params::get_int>     out_int;
    flxParameterOutFloat<test_params, &test_params::get_float> out_float;
    flxParameterOutString<test_params, &test_params::get_str>  out_string;

    flxParameterInBool<test_params, &test_params::set_bool>   in_bool;
    flxParameterInInt<test_params, &test_params::set_int>     in_int = {0, 144};

    flxParameterInFloat<test_params, &test_params::set_float> in_float = {
                                            {"Value One", 10.},
                                            {"Value Two", 22.},
                                            {"Value Three", 33.},        
                                            {"Value Four", 44.},      
                                            {"Value Five", 155.}                       
                                        };
    flxParameterInString<test_params, &test_params::set_str>  in_string;  

    flxParameterInVoid<test_params, &test_params::set_void>   in_void;   
};

// Method implementation

// boolean setter/getter
bool test_params::get_bool(void){
    return _b_data;
}
void test_params::set_bool(const bool &b){

    Serial.printf("\n\r\n\r\t[TEST: Bool set to: %d]\n\r", b);
    _b_data=b;
};

// int setter/getter
int test_params::get_int(void){
    return _i_data;
}
void test_params::set_int(const int &data){
    Serial.printf("\n\r\n\r\t[TEST: Int set to: %d]\n\r", data);    
    _i_data=data;
};

// float setter/getter
float test_params::get_float(void){
    return _f_data;
}
void test_params::set_float(const float &data){
    Serial.printf("\n\r\n\r\t[TEST: Float set to: %f]\n\r", data);        
    _f_data=data;
};

// str setter/getter
std::string test_params::get_str(void){
    return _s_data;
}
void test_params::set_str(const std::string &data){
    Serial.printf("\n\r\n\r\t[TEST: String set to: `%s`]\n\r", data.c_str());        
    _s_data=data;    
}

void test_params::set_void(void){
    Serial.printf("\n\r\n\r\t[TEST: VOID set called]\n\r");
}
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


flxLogger  logger;

// Enable a timer with a default timer value - this is the log interval
flxTimer   timer(3000);    // Timer 

flxStorageESP32Pref  myStorage;
flxSettingsSerial    serialSettings;


test_params testParams;
test_properties testProps;
//---------------------------------------------------------------------
// Arduino Setup
//
void setup() {

    // Begin setup - turn on board LED during setup.
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); 

    Serial.begin(115200);  
    while (!Serial);
    Serial.println("\n---- Startup Serial Settings Test ----");
    

    flxSettings.setStorage(myStorage);
    // Have settings save when editing is complete.
    flxSettings.listenForSave(serialSettings.on_finished);

    // Start Spark 
    spark.start();  

    // our testing parameter and property objects.
    spark.add(testParams);

    spark.add(testProps);

    // What's connected
    flxDeviceContainer  myDevices = spark.connectedDevices();

    Serial.printf("Number of Devices Detected: %d\r\n", myDevices.size() );

    // Loop over the device list - note that it is iterable. 
    for (auto device: myDevices )
    {
        Serial.printf("Device: %s, Output Number: %d\n\r", device->name(), device->nOutputParameters()); 
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    // Add serial settings to spark - the spark loop call will take care
    // of everything else.
    spark.add(serialSettings);
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
    spark.loop();

    // Our loop delay 
    delay(500);                       
    //digitalWrite(LED_BUILTIN, LOW);   // turn off the log led
    //delay(500);
}

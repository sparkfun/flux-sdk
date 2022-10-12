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

#include <Spark/spSettingsSerial.h>
#include <Spark/spSettingsSave.h>
#include <Spark/spStorageESP32Pref.h>


class test_properties : public spActionType<test_properties>
{

    bool   _b_data=false;
    int    _i_data=0;
    float  _f_data=0.;    
    std::string _s_data="";

public:

    test_properties(){

        setName("Test Properties", "Property testing object");

        spRegister(prop_bool);
        
        spRegister(prop_int, "MyInteger", "Testing integer property - with a range limit");
        prop_int.setDataLimit(int_limit);

        spRegister(prop_float, "FloatValue");
        spRegister(prop_str, "stringProp", "Testing a StringProperty");        

        spRegister(rw_prop_bool);
        
        // Register the RW int property and add the data limit set.
        spRegister(rw_prop_int, "rw_int", "Testing Read/Write integer property with data limit set");
        rw_prop_int.setDataLimit(rw_init_limit);

        spRegister(rw_prop_str);
        
        spRegister(rw_prop_float, "RW Float", "Float property with a range limit");
        rw_prop_float.setDataLimit(float_limit);
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
    spPropertyBool<test_properties>     prop_bool;

    // int property, that we'll add a range limit to. 
    spPropertyInt<test_properties>      prop_int;
    spDataLimitRangeInt  int_limit = {33, 44};   // define the limit, set it in the constructor

    spPropertyFloat<test_properties>    prop_float;
    spPropertyString<test_properties>   prop_str;

    // Define RW (getter/setter) Properties
    spPropertyRWBool<test_properties, &test_properties::get_bool, &test_properties::set_bool> rw_prop_bool;
    
    spPropertyRWInt<test_properties, &test_properties::get_int, &test_properties::set_int> rw_prop_int;    
    // Add a list of available values for this property. These are name (human readable), value pairs
    spDataLimitSetInt rw_init_limit = {
        {"Value One", 111},
        {"Value Two", 222},
        {"Value Three", 333},        
        {"Value Four", 444},      
        {"Value Five", 555}                       
    };
    spPropertyRWFloat<test_properties, &test_properties::get_float, &test_properties::set_float> rw_prop_float; 
    spDataLimitRangeFloat float_limit = {-100, 100, 22}; // only needs 2 values, but making sure system accepts 3 - skips #3

    spPropertyRWString<test_properties, &test_properties::get_str, &test_properties::set_str> rw_prop_str;   



};

// Define an action class that uses parameters for testing

class test_params : public spActionType<test_params>
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
        in_int.setDataLimit(int_limit);

        spRegister(in_string, "in string", "test input of a string value");
        spRegister(in_float, "in float", "test input of a float value");
        spRegister(in_void, "in void", "test input parameter of type void");
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

    // VOID
    void set_void(void);


    // Output Parameters
    spParameterOutBool<test_params, &test_params::get_bool>   out_bool;
    spParameterOutInt<test_params, &test_params::get_int>     out_int;
    spParameterOutFloat<test_params, &test_params::get_float> out_float;
    spParameterOutString<test_params, &test_params::get_str>  out_string;

    spParameterInBool<test_params, &test_params::set_bool>   in_bool;
    spParameterInInt<test_params, &test_params::set_int>     in_int;
    spDataLimitRangeInt  int_limit = {0, 144};   // define the limit, set it in the constructor

    spParameterInFloat<test_params, &test_params::set_float> in_float;
    spParameterInString<test_params, &test_params::set_str>  in_string;  

    spParameterInVoid<test_params, &test_params::set_void>   in_void;   
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


spLogger  logger;

// Enable a timer with a default timer value - this is the log interval
spTimer   timer(3000);    // Timer 

spSettingsSerial    serialSettings;

// Create a save settings action, passing in the ESP32 Pref object
// as a storage destination

spStorageESP32Pref  settingsStorage;
spSettingsSave      saveSettings(settingsStorage);

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
    

    // Have settings save when editing is complete.
    saveSettings.listenForSave(serialSettings.on_finished);

    // Add the save system to the app
    spark.add(saveSettings);
    
    // Start Spark 
    spark.start();  

    // our testing parameter and property objects.
    spark.add(testParams);

    spark.add(testProps);

    // What's connected
    spDeviceContainer  myDevices = spark.connectedDevices();

    Serial.printf("Number of Devices Detected: %d\r\n", myDevices.size() );

    // Loop over the device list - note that it is iterable. 
    for (auto device: myDevices )
    {
        Serial.printf("Device: %s, Output Number: %d\n\r", device->name(), device->nOutputParameters());
        
    }

    digitalWrite(LED_BUILTIN, LOW);  // board LED off

    // Set the settings system to start at root of the spark system.
    serialSettings.setSystemRoot(&spark);

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

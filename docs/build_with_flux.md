
# Building with Flux {#building-with-flux}

This section outline the steps needed to support Flux in an Arduino Build Environment. Both support for Arduino IDE development, as well as automated builds that use the Arduino CLI (GitHub actions).

Unlike standard Arduino Libraries, the flux-sdk uses the `cmake` build system to create a custom Arduino Library for the target application. This allows the specification of a target platform ias well as the specific features and functionality used in the target application.

The cmake build system creates the target Arduino Library, the library is either located in builds systems Arduino library directory (normally in the users Documentation/Arduino/libraries folder) or specified using the ```--library``` option to the ```arduino-cli``` tool.

## Building an Application Specific flux-sdk Library

A custom library is configured and built using the standard build tool `cmake`

### Install the flux sdk

The flux sdk is used to create a custom Arduino Library called ```SparkFun_Flux``` that is made available for use by the Arduino build environment. This allows tailoring which components are needed for the specific application - in this case the datalogger.

First steps is to download the flux-sdk on your machine - which is cloned from github.

```sh
git clone git@github.com:sparkfun/flux-sdk.git
```

The normal location is to set install the flux-sdk in the same directory as the ```sfe-dataloger``` (this) repository was installed.  If you install it some other location, set the flux-sdk path using the FLUX_SDK_PATH environment variable. This is easily done by cd'ing into the flux-sdk root directory and executing the following command:

```sh
export FLUX_SDK_PATH=`pwd`
```

### Configure cmake

To specify what components of flux are used by a project, a ```CMakeLists.txt``` file is used. This file is placed in the root of your project folder and outlines what modules of the flux-sdk to use make available.

The following is an example of a cmake file from the SparkFun DataLogger IoT application:

```cmake

cmake_minimum_required(VERSION 3.13)

# set project name - setting language to NONE disables the default compiler checks
project(DataLoggerIoT NONE)

# Import the flux-dk cmake system
include(flux_sdk_import.cmake)

# Where is directory that the flux stuff will be added to? This is the relative path from this file
# to the arduino sketch directory this is also used as the name of the cmake project
flux_sdk_set_project_directory(.)

# datalogger is esp32 based
flux_sdk_set_platform(platform_esp32)

# Currently we are using all modules of the SDK
flux_sdk_add_module(flux_all_modules)

# now call the init function/macro - this will build the Arduino Library SparkFun_Flux under this
# main directory
flux_sdk_init()

```

The key components of this file are the following include the standard cmake version check, and then setting the project name using this line:

```cmake
# set project name - setting language to NONE disables the default compiler checks
project(DataLoggerIoT NONE)
```

After the project is defined, the cmake flux-sdk functions and configuration routines are included as follows:

```cmake
# Import the flux-dk cmake system
include(flux_sdk_import.cmake)
```

The file `flux_sdk_import.cmake` is located in `external` folder of the flux-sdk ($FLUX_SDK/external) and should be copied to the same folder that contains your projects CMakeLists.txt file.

The next line sets the location of the project. In this example, it's just the current working directory.

```cmake
# Where is directory that the flux stuff will be added to? This is the relative path from this file
# to the arduino sketch directory this is also used as the name of the cmake project
flux_sdk_set_project_directory(.)
```

The platform used for the build is set using the following line (note, currently the flux-sdk is only supported on ESP32):

```cmake
# datalogger is esp32 based
flux_sdk_set_platform(platform_esp32)
```

The flux-sdk components or `modules` that are included in the application are specified using the `flux_sdk_add_module()` call. The modules correspond to folder names within the `src` folder of the sdk - for example to add the BME280 sensor device, the module name is `device_bme280`.

To add everything, the name `flux_all_modules` is used, and shown in the example above.

```cmake
# Currently we are using all modules of the SDK
flux_sdk_add_module(flux_all_modules)
```

The last step in the cmake file is calling the sdk init function `flux_sdk_init()`.

```cmake
# now call the init function/macro - this will build the Arduino Library SparkFun_Flux under this
# main directory
flux_sdk_init()
```

### Building the custom Flux Arduino Library

 configure the library used during the Arduino build process, the ```cmake``` system is used. The following steps outline how the custom library is built.

Set the current directory the root of your project. Then create a directory called build and cd into it.

```sh
mkdir build
cd build
```

Now run CMake with the following command:

```sh
cmake ..
```

This will create an Arduino library called ```SparkFun_Flux``` in the root directory of the project. Once completed, you can delete the build directory, and build the projects firmware.

This custom library can be copied to the Arduino libraries install folder and used like a standard Arduino library, or referenced using the ```---library``` flag of the arduion-cli command

## Building using the Arduino CLI

### Install Arduino CLI

First, install the ```arduino-cli``` on your system. Details on how to do this are located [here](https://arduino.github.io/arduino-cli/0.20/installation/). While the arduino-cli isn't required to configure or build a flux build Arduino library, it helps with configuration.

#### Install the Target Arduino Core

Once the CLI is installed, the target core for the application can be installed using the arduino-cli. Note: this can also be performed within the Arduino application itself.

For example,the following commands complete the installation and install the ESP32 Arduino platform

```sh
arduino-cli config init --additional-urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"

arduino-cli core update-index

arduino-cli core install esp32:esp32
```

### Install dependant Arduino libraries

Once the flux-sdk is installed, you can install all libraries the framework depends on by issuing the following command (note this command uses the ```arduino-cli```):

```sh
$FLUX_SDK_PATH/install-libs.sh
```

### Compile and Build

Once all the dependencies are installed, the ```arduino-cli compile``` option is called to build the desired application. To use Flux as a library, the ```--library``` switch is used with the compile call.

The following is an example of building an ESP32 based sketch (the SparkFun DataLogger Application), which uses the custom Flux library.

Note that the location of the Flux library is passed in using the ```--library'`` switch, and that the ***full*** path to the Flux directory is provided. Using a relative path to the Flux library directory causes this command to fail

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 ./sfeDataLoggerIoT/sfeDataLoggerIoT.ino  \
            --export-binaries --library `pwd`/SparkFun_Flux     
```

Once the build is complete, the resultant binary files are located in: `sfeDataLoggerIoT/build/esp32.esp32.esp32/`

## Further Details

A great example of how to build a flux-sdk application at the command line, as well as part of a github action is the SparkFun DataLogger IoT application, located [here](https://github.com/sparkfun/sfe-datalogger)

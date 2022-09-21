curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_MCP9600_Arduino_Library/master/src/SparkFun_MCP9600.h
curl -O https://raw.githubusercontent.com/sparkfun/SparkFun_MCP9600_Arduino_Library/master/src/SparkFun_MCP9600.cpp
sed -i 's/<SparkFun_MCP9600.h>/'\"'SparkFun_MCP9600.h'\"'/g' SparkFun_MCP9600.cpp

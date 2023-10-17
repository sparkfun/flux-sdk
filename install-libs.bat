@echo off
:: Batch file to load the needed arduino libraries on windows
:: Note: this should be run in the root directory of flux - it's a fragile script
:: Note: Arduino-cli needs to be installed 

for /f "tokens=*" %%a in (library-list.txt) do (
  arduino-cli.exe lib install %%a
)

@echo off
REM Change current directory to the [d]rive and [p]ath the batch file is in
pushd "%~dp0"
REM Get parameters off user
set /p param1=Enter Start Frequency(88.0, 97.6)  :  
set /p param2=Enter Number of stations to create(10, 20, etc.) : 
set /p param3=Enter region code (DE, GB, etc.)  :  
if "%param1%"=="" (
  echo You must enter the start frequency parameter
  exit /b
)
if "%param2%"=="" (
  echo You must enter the number of stations to create parameter
  exit /b
)
if "%param3%"=="" (
  echo You must enter the region code
  exit /b
)

REM Run MultiFM_StationCreator.dll with hard coded into batch parameters
MultiFM_StationCreator.exe "%param1%" "%param2%" "%param3%"
popd
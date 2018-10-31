@echo off
REM Change current directory to the [d]rive and [p]ath the batch file is in
pushd "%~dp0"
REM Get parameters off user
set /p param1=Enter Start Frequency(range is 88.x - 108.x)  :  
set /p param2=Enter Number of stations to create(1, 5, 10, 20, etc.) : 
set /p param3=Enter one of the given region code(AD, AL, AT, BA, BE, BG, BY, CH, CY, CZ, DE, DK, EE, ES, FI, FO, FR, GB, GI, GR, HR, HU, IE, IS, IT, LI, LT, LU, LV, MC, MD, ME, MK, MT, NL, NO, PL, PT, RO, RS, RU, SE, SI, SK, SM, TR, UA, VA) :
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
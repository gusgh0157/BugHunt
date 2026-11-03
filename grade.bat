@echo off
rem Usage: grade.bat [Debug^|Release^|ASan] [lab number ^| C-1 ^| labs ^| challenges]   (default: Debug, everything)
rem Builds, then runs every lab in its own process and writes grade-report.json here.
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug

call "%~dp0build.bat" %CONFIG% >nul || (echo Build failed. Run build.bat %CONFIG% to see the errors. & exit /b 1)

pushd "%~dp0"
"%~dp0bin\%CONFIG%\BugHunt.exe" --grade %2
set RESULT=%ERRORLEVEL%
popd
exit /b %RESULT%

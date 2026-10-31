@echo off
rem Usage: build.bat [Debug^|Release^|ASan]   (default: Debug)
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
"%VSWHERE%" -latest -property installationPath > "%TEMP%\bughunt_vsdir.txt" || exit /b 1
set /p VSDIR=<"%TEMP%\bughunt_vsdir.txt"
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>nul || exit /b 1

msbuild "%~dp0BugHunt\BugHunt.vcxproj" /nologo /v:minimal /p:Configuration=%CONFIG% /p:Platform=x64 || exit /b 1
echo.
echo Built: %~dp0bin\%CONFIG%\BugHunt.exe

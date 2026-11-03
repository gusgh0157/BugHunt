@echo off
rem Usage: build.bat [Debug^|Release^|ASan]   (default: Debug)
rem Builds with CMake (CMakePresets.json) and the compiler of the Visual Studio developer environment.
rem Output: bin\<config>\BugHunt.exe
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug
set PRESET=
if /i "%CONFIG%"=="Debug" set PRESET=debug
if /i "%CONFIG%"=="Release" set PRESET=release
if /i "%CONFIG%"=="ASan" set PRESET=asan
if "%PRESET%"=="" (echo Unknown configuration: %CONFIG%. Use Debug, Release or ASan. & exit /b 1)

rem Outside a developer prompt, find Visual Studio and load its environment (cl, cmake, ninja).
where cl >nul 2>nul
if errorlevel 1 (
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    call :loadvs || exit /b 1
)

cmake --preset %PRESET% || exit /b 1
cmake --build --preset %PRESET% || exit /b 1
echo.
echo Built: %~dp0bin\%CONFIG%\BugHunt.exe
exit /b 0

:loadvs
"%VSWHERE%" -latest -property installationPath > "%TEMP%\bughunt_vsdir.txt" || exit /b 1
set /p VSDIR=<"%TEMP%\bughunt_vsdir.txt"
call "%VSDIR%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>nul || exit /b 1
set "PATH=%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
exit /b 0

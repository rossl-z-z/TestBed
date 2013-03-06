@echo off
set OGRE_HOME=d:\dev\OGRE
set CEGUI_HOME=d:\dev\CEGUI-0.7.7


rem Specify the path to your CMake installation
rem set CMAKE=C:\Program Files\CMake 2.8
rem For x64 the following might work for you:
set CMAKE=C:\Program Files (x86)\CMake 2.8

rem Choose a build type (debug, release);
rem uncomment either of the following:
rem set BUILD=RelWithDebInfo
rem set BUILD=Release
set BUILD=Debug


rem ---------------------------------------------------------
rem Preparing environment...
set PATH=%PATH%;%CMAKE%\bin
rem Calling CMake...
cmake.exe -G "Visual Studio 11" . -DCMAKE_BUILD_TYPE="%BUILD%"

rem Compile
echo Compile! TODO

echo 
echo All done.
pause

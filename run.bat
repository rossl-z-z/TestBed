@echo off
set OGRE_HOME=d:\dev\OGRE

mingw32-make
if errorlevel 1 goto done

cd dist/bin
FOR /F %%I IN ('DIR TestBed*.* /B /O:-D') DO SET newest=%%I
%newest%
cd ..
cd ..

:done
echo All done.
rem pause

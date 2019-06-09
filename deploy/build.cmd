SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\
SET QT=D:\Projects\Libraries\Qt\5.12.3\msvc2017_64\bin\
SET BASE=D:\Projects\MapleSeed\
SET RELEASE=%BASE%bin\release\
SET DEPLOY=%BASE%deploy\
SET BINARIES=%BASE%..\Binaries\

FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (SET fVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (SET ccVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (SET cVERSION=%%A)
SET FILEOUT=MapleSeed-%fVERSION%.%ccVERSION%

%QT%windeployqt --dir %RELEASE% --plugindir %RELEASE%plugins %RELEASE%MapleSeed.exe
copy C:\OpenSSL-Win64\bin\libcrypto-1_1-x64.dll %RELEASE%libcrypto-1_1-x64.dll
copy D:\Projects\MapleSeed\deploy\titlekeys.json %RELEASE%titlekeys.json
copy D:\Projects\MapleSeed\deploy\XInput1_4.dll %RELEASE%XInput1_4.dll
del /S %RELEASE%Makefile
del /S %RELEASE%*.Debug
del /S %RELEASE%*.Release
del /S %RELEASE%*.cpp
del /S %RELEASE%*.h
del /S %RELEASE%*.o
del /S %RELEASE%*.obj

CALL iscc /O"%BINARIES%" /F"%FILEOUT%" "%DEPLOY%mapleseed.iss"
CALL 7z -x!*.lnk -x!*.zip -x!MapleSeed-*.exe -xr!libs a "%BINARIES%%FILEOUT%.zip" %RELEASE%*
::cmd /k
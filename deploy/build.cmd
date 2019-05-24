::SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\
SET RELEASE=D:\Projects\MapleSeed\release\
SET DEPLOY=D:\Projects\MapleSeed\deploy\
SET BINARIES=%RELEASE%..\..\Binaries\

FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (SET fVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (SET ccVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (SET cVERSION=%%A)
SET FILEOUT=MapleSeed-%fVERSION%.%ccVERSION%

windeployqt --dir %RELEASE% --plugindir %RELEASE%plugins %RELEASE%MapleSeed.exe
copy C:\OpenSSL-Win64\bin\libcrypto-1_1-x64.dll %RELEASE%libcrypto-1_1-x64.dll
del /S %RELEASE%Makefile
del /S %RELEASE%*.Debug
del /S %RELEASE%*.Release
del /S %RELEASE%*.cpp
del /S %RELEASE%*.h
del /S %RELEASE%*.o

CALL iscc /O"%BINARIES%" /F"%FILEOUT%" "%DEPLOY%mapleseed.iss"
CALL 7z -x!*.lnk -x!*.zip -x!MapleSeed-*.exe -xr!libs a "%BINARIES%%FILEOUT%.zip" %RELEASE%*
::cmd /k
SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\
SET RELEASE=D:\Projects\MapleSeed\release\
SET DEPLOY=D:\Projects\MapleSeed\deploy\
SET BINARIES=%RELEASE%..\..\Binaries\
SET QT=D:\Projects\Libraries\Qt\5.12.3-msvc2019-x86_64
FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (SET fVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (SET ccVERSION=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (SET cVERSION=%%A)
SET FILEOUT=MapleSeed-%fVERSION%-%ccVERSION%-%cVERSION%

windeployqt --dir %RELEASE% --plugindir %RELEASE%plugins %RELEASE%MapleSeed.exe
::copy C:\openssl11\bin64\libcryptoMD.dll libcryptoMD.dll
::copy %DEPLOY%\titlekeys.json %RELEASE%titlekeys.json
::copy %DEPLOY%\titles.json %RELEASE%titles.json
del /S %RELEASE%moc_*.cpp
del /S %RELEASE%moc_*.h

CALL iscc /O"%BINARIES%" /F"%FILEOUT%" "%DEPLOY%mapleseed.iss"
CALL 7z -x!*.lnk -x!*.zip -x!MapleSeed-*.exe -xr!libs a "%BINARIES%%FILEOUT%.zip" %RELEASE%* C:\openssl11\bin64\libcryptoMD.dll
::cmd /k
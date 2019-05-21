SET RELEASE=%CD%\release\
SET DEPLOY=%CD%\deploy\
SET QT=D:\Projects\Libraries\Qt\5.12.2-dynamic-msvc2017-x86_64
CD %RELEASE%
CALL %QT%\bin\windeployqt.exe MapleSeed.exe
::copy %DEPLOY%\titlekeys.json titlekeys.json
::copy %DEPLOY%\titles.json titles.json
rmdir /s /q plugins
mkdir plugins
move bearer plugins
move imageformats plugins
move platforms plugins
move styles plugins
move translations plugins
move iconengines plugins
del /S moc_*.cpp
del /S moc_*.h

FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (
  SET fVERSION=%%A
)

FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (
  SET cVERSION=%%A
)

FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (
  SET ccVERSION=%%A
)

SET FILEOUT=MapleSeed-%fVERSION%-%ccVERSION%-%cVERSION%
CALL "C:\Program Files\7-Zip\7z.exe" a %FILEOUT%.zip *
CALL "C:\Program Files (x86)\Inno Setup 6\iscc.exe" /O"%RELEASE%" /F"%FILEOUT%" "%DEPLOY%mapleseed.iss"
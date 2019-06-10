@ECHO OFF
SETLOCAL

SET MS=D:\Projects\MapleSeed\
SET resourceFile=%MS%resources\mapleseed.rc
SET versionFile=%MS%versioninfo.h

FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (SET fileVersion=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (SET commitVersion=%%A)
FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (SET commitCount=%%A)

SET versionString=%fileVersion%.%commitCount%

ECHO //generated resource header > "%versionFile%"
ECHO #define GEN_LATEST_VERSION_STRING "%fileVersion%\0" >> "%versionFile%"
ECHO #define GEN_COMMIT_STRING "%commitVersion%\0" >> "%versionFile%"
ECHO #define GEN_COMMIT_COUNT_STRING "%commitCount%\0" >> "%versionFile%"
ECHO #define GEN_VERSION_STRING "%versionString%\0" >> "%versionFile%"

ECHO //generated resource header > "%resourceFile%"
ECHO #include ^<windows.h^> >> "%resourceFile%"
ECHO IDI_ICON1 ICON "sprout.ico" >> "%resourceFile%"
ECHO VS_VERSION_INFO VERSIONINFO >> "%resourceFile%"
ECHO FILEVERSION 1, 0, 0, %commitCount% >> "%resourceFile%"
ECHO PRODUCTVERSION 1, 0, 0, %commitCount% >> "%resourceFile%"
ECHO FILEFLAGSMASK 0x3fL >> "%resourceFile%"
ECHO #ifdef _DEBUG >> "%resourceFile%"
ECHO FILEFLAGS 0x1L >> "%resourceFile%"
ECHO #else >> "%resourceFile%"
ECHO FILEFLAGS 0x0L >> "%resourceFile%"
ECHO #endif >> "%resourceFile%"
ECHO FILEOS 0x40004L >> "%resourceFile%"
ECHO FILETYPE 0x0L >> "%resourceFile%"
ECHO FILESUBTYPE 0x0L >> "%resourceFile%"
ECHO BEGIN >> "%resourceFile%"
ECHO BLOCK "StringFileInfo" >> "%resourceFile%"
ECHO BEGIN >> "%resourceFile%"
ECHO BLOCK "040904b0" >> "%resourceFile%"
ECHO BEGIN >> "%resourceFile%"
ECHO VALUE "CompanyName", "Maple-Tree" >> "%resourceFile%"
ECHO VALUE "FileDescription", "Cemu Content Manager" >> "%resourceFile%"
ECHO VALUE "FileVersion", "%versionString%" >> "%resourceFile%"
ECHO VALUE "InternalName", "MapleSeed" >> "%resourceFile%"
ECHO VALUE "LegalCopyright", "Copyright (C) 2019" >> "%resourceFile%"
ECHO VALUE "OriginalFilename", "MapleSeed.exe" >> "%resourceFile%"
ECHO VALUE "ProductName", "MapleSeed C++" >> "%resourceFile%"
ECHO VALUE "ProductVersion", "%versionString%" >> "%resourceFile%"
ECHO END >> "%resourceFile%"
ECHO END >> "%resourceFile%"
ECHO BLOCK "VarFileInfo" >> "%resourceFile%"
ECHO BEGIN >> "%resourceFile%"
ECHO VALUE "Translation", 0x409, 1200 >> "%resourceFile%"
ECHO END >> "%resourceFile%"
ECHO END >> "%resourceFile%"

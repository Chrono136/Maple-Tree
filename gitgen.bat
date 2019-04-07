@ECHO OFF
SETLOCAL

FOR /F "tokens=*" %%A IN ('"git describe --tags --abbrev=0"') DO (
  SET strFILE_VERSION=%%A
)

FOR /F "tokens=*" %%A IN ('"git rev-parse --short HEAD"') DO (
  SET strCOMMIT_VERSION=%%A
)

FOR /F "tokens=*" %%A IN ('"git rev-list --all --count"') DO (
  SET strCOMMIT_COUNT_VERSION=%%A
)

SET HEADER_OUT_FILE=%~fs1

ECHO //generated resource header.>"%HEADER_OUT_FILE%"
ECHO #define GEN_LATEST_VERSION_STRING "%strFILE_VERSION%\0" >> "%HEADER_OUT_FILE%"
ECHO #define GEN_COMMIT_STRING "%strCOMMIT_VERSION%\0" >> "%HEADER_OUT_FILE%"
ECHO #define GEN_COMMIT_COUNT_STRING "%strCOMMIT_COUNT_VERSION%\0" >> "%HEADER_OUT_FILE%"
ECHO #define GEN_VERSION_STRING "%strFILE_VERSION%-%strCOMMIT_COUNT_VERSION% [%strCOMMIT_VERSION%]\0" >> "%HEADER_OUT_FILE%"
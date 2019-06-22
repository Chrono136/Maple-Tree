SET VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC
SET BASE=D:\Projects\MapleSeed\
SET RELEASE=%BASE%bin\release\
windeployqt --dir %RELEASE% --plugindir %RELEASE%plugins %RELEASE%MapleSeed.exe
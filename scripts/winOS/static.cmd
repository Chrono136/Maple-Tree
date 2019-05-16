SET QT5=D:\Projects\Libraries\qt-everywhere-src-5.12.2
SET prefix_folder=%QT5%\..\Qt\5.12.2-static-msvc2017-x86_64

mkdir %QT5%\config-env
CD %QT5%\config-env

SET QT_SRC=%QT5%\src
SET OPENSSL_BASE=C:\openssl11
SET OPENSSL_INCLUDE=%OPENSSL_BASE%\include64
SET OPENSSL_LIB=%OPENSSL_BASE%\lib64
::SET LLVM_INSTALL_DIR=D:\Projects\Libraries\libclang_7.0
SET PATH=C:\;%QT_SRC%\qtbase\bin;%QT_SRC%\gnuwin32\bin;%QT_SRC%;%PATH%;

%QT_SRC%\configure -v -debug-and-release -opensource -confirm-license -platform win32-msvc2017 -opengl desktop -no-iconv -no-dbus -no-icu -no-fontconfig -no-freetype -qt-harfbuzz -nomake examples -nomake tests -skip qt3d -skip qtactiveqt -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdatavis3d -skip qtdoc -skip qtgamepad -skip qtcharts -skip qtgraphicaleffects -skip qtlocation -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtvirtualkeyboard -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview -skip qtscript -skip qtxmlpatterns -mp -optimize-size -D "JAS_DLL=0" -static -static-runtime -prefix %prefix_folder% -ltcg
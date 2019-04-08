cmake_minimum_required(VERSION 3.12.2)

project(MapleSeed++)

set(CMAKE_CONFIGURATION_TYPES "Debug"
    CACHE STRING "Configuration types" FORCE)

add_executable(MapleSeed++
  DownloadClient.cpp
  DownloadClient.h
  gen-versioninfo.h
  main.cpp
  MapleMini.cpp
  MapleMini.h
  MapleTicket.cpp
  MapleTicket.h
  stdafx.cpp
  stdafx.h
  targetver.h
  ThirdParty/cdecrypt.cpp
  ThirdParty/frozen.c
  ThirdParty/frozen.h
  ThirdParty/rang.hpp
  ThreadManager.cpp
  ThreadManager.h
  TitleInfo.cpp
  TitleInfo.h
  Tools.cpp
  Tools.h
  UIMain.cpp
  UIMain.h
)
# Output file name
set_target_properties(MapleSeed++
  PROPERTIES
  OUTPUT_NAME_DEBUG MapleSeed++_x64D
)

# Preprocessor definitions
target_compile_definitions(MapleSeed++ PRIVATE
  $<$<CONFIG:Debug>:_UNICODE;_DEBUG;_CONSOLE;_CRT_SECURE_NO_WARNINGS>
)

# SDL check
target_compile_options(MapleSeed++ PRIVATE
  "$<$<CONFIG:Debug>:/sdl>"
)

# Minimal rebuild
if (MSVC)
  target_compile_options(MapleSeed++ PRIVATE
    "$<$<CONFIG:Debug>:/Gm->"
  )
endif ()

# Precompiled header files
if (MSVC)
  set_property(SOURCE ThreadManager.cpp Tools.cpp MapleTicket.cpp DownloadClient.cpp TitleInfo.cpp UIMain.cpp main.cpp MapleMini.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Debug>:/Yu>")
  set_property(SOURCE stdafx.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Debug>:/Yc>")
endif ()

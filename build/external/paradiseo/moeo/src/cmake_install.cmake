# Install script for directory: /Users/vguillem/git/rHashGen/external/paradiseo/moeo/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libraries" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/Users/vguillem/git/rHashGen/build/lib/libmoeo.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libmoeo.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libmoeo.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libmoeo.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/paradiseo/moeo" TYPE FILE FILES "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/moeo")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/paradiseo/moeo" TYPE DIRECTORY FILES
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/acceptCrit"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/algo"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/archive"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/continue"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/comparator"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/core"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/distance"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/diversity"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/do"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/explorer"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/fitness"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/hybridization"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/metric"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/replacement"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/scalarStuffs"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/selection"
    "/Users/vguillem/git/rHashGen/external/paradiseo/moeo/src/utils"
    FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()


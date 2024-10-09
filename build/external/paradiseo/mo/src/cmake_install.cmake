# Install script for directory: /Users/vguillem/git/rHashGen/external/paradiseo/mo/src

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/paradiseo/mo" TYPE FILE FILES
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/mo"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/mo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/paradiseo/mo" TYPE DIRECTORY FILES
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/acceptCrit"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/algo"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/comparator"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/continuator"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/coolingSchedule"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/eval"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/explorer"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/memory"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/neighborhood"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/perturb"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/problems"
    "/Users/vguillem/git/rHashGen/external/paradiseo/mo/src/sampling"
    FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()


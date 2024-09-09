cmake_minimum_required(VERSION 3.10)

# Project name
project(rHashGen)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


######################################################################################
# Configurable user settings
######################################################################################

# Compilation options
option(BUILD_DOCS "Enable building of documentation" OFF)
option(BUILD_TESTING "Enable building of tests" OFF)
option(BUILD_FOR_LOCAL "Whether to make the executables dependant on the environment of the building computer (enables CMake's build with RPATH), may be necessary on HPC clusters" OFF)
option(USE_LOCAL_PARADISEO "Use a local version of Paradiseo rather than the one installed on the system" ON)

# Common
add_compile_options(-Wall -Wextra -pedantic)
# Clang
#add_compile_options( -Wno-c++98-compat-pedantic -Wno-old-style-cast -Wno-padded -Wno-extra-semi-stmt -Wno-weak-vtables)

add_compile_definitions(-DWITH_CLUTCHLOG)

if(BUILD_FOR_LOCAL)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
endif()


######################################################################################
# Dependencies
######################################################################################

# Own lib
include_directories(include)

# ParadisEO
if(USE_LOCAL_PARADISEO)
    set(PARADISEO_ROOT "${PROJECT_SOURCE_DIR}/external/paradiseo" CACHE PATH "Where to find ParadisEO")
    set(PARADISEO_BUILD "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "Build dir of ParadisEO")

    include_directories(${PARADISEO_ROOT})
    include_directories(${PARADISEO_ROOT}/eo/src)
    include_directories(${PARADISEO_ROOT}/mo/src)
    link_directories(${PARADISEO_BUILD}/lib)
else()
    include_directories($ENV{PARADISEO_ROOT}/include/paradiseo/eo)
    include_directories($ENV{PARADISEO_ROOT}/include/paradiseo/mo)
    link_directories($ENV{PARADISEO_ROOT}/lib64)
endif()
set(PARADISEO_LIBRARIES eoutils eo)

# Single-header dependencies.
include_directories(external/clutchlog)

# Include the src directory
include_directories(${PROJECT_SOURCE_DIR}/src/include)


######################################################################################
# Start building
######################################################################################

# Add subdirectories
add_subdirectory(src)

# Google Test
add_subdirectory(external/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# ParadisEO
add_subdirectory(external/paradiseo)


# Find all main files in the app directory
file(GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/app/*.cpp)

# For each main file, create an executable
foreach(APP_SOURCE ${APP_SOURCES})
    # Get the file name without the extension
    get_filename_component(APP_NAME ${APP_SOURCE} NAME_WE)

    # Add the executable
    add_executable(${APP_NAME} ${APP_SOURCE})

    # Link the necessary libraries
    target_link_libraries(${APP_NAME} PRIVATE rHashGenLib ${PARADISEO_LIBRARIES})
endforeach()

# Add the test directory
enable_testing()
add_subdirectory(test)

cmake_minimum_required(VERSION 3.10)

# Project name
project(rHashGen)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include the src directory
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# Add subdirectories
add_subdirectory(src)

# Find all main files in the app directory
file(GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/app/*.cpp)

# For each main file, create an executable
foreach(APP_SOURCE ${APP_SOURCES})
    # Get the file name without the extension
    get_filename_component(APP_NAME ${APP_SOURCE} NAME_WE)
    
    # Add the executable
    add_executable(${APP_NAME} ${APP_SOURCE})
    
    # Link the necessary libraries
    target_link_libraries(${APP_NAME} PRIVATE rHashGenLib)
endforeach()

# Add source files in src
file(GLOB_RECURSE SRC_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

# Create a static or shared library
add_library(rHashGenLib ${SRC_SOURCES})

# Specify the linker language for the library
set_target_properties(rHashGenLib PROPERTIES LINKER_LANGUAGE CXX)
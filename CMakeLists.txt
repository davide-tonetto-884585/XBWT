cmake_minimum_required(VERSION 3.10)

# Set the project name
project(XBWT)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the XBWT library
add_library(XBWT_lib XBWT/XBWT.cpp XBWT/XBWT.hpp)

# Add the LabeledTree library
add_library(LabeledTree_lib LabeledTree/LabeledTree.cpp LabeledTree/LabeledTree.hpp)

# Add the executable
add_executable(XBWT main.cpp)

# Link the XBWT library to the executable
target_link_libraries(XBWT PRIVATE XBWT_lib)

# Link the LabeledTree library to the executable
target_link_libraries(XBWT PRIVATE LabeledTree_lib)

target_link_libraries(XBWT PRIVATE sdsl)

# Enable testing
enable_testing()

# Add a test using Valgrind
add_test(NAME memcheck
         COMMAND valgrind --leak-check=full --error-exitcode=1 $<TARGET_FILE:XBWT>)

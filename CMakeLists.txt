#************************************************************************************
# 4/30/2016
# Randy Direen
#
# Build system for Rabit
#
#************************************************************************************
cmake_minimum_required (VERSION 2.8)
project (rabit_dev)

#************************************************************************************
# Setup Versioning
#************************************************************************************

# The version number.
set(rabit_VERSION_MAJOR 1)
set(rabit_VERSION_MINOR 0)
set(rabit_VERSION_PATCH 1)
set(rabit_VERSION "${rabit_VERSION_MAJOR}.${rabit_VERSION_MINOR}.${rabit_VERSION_PATCH}")


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/rabitConfig.h.in"
  "${PROJECT_BINARY_DIR}/rabitConfig.h"
  )
  

option(WITHTESTS "Compile code with unit tests and integration tests" OFF)
option(WITHEXAMPLES "Compile code with examples" OFF)
option(WITHCOVERAGE "Compile code with code coverage (for online CI)" OFF)

#set(WITHTESTS true)
#set(WITHEXAMPLES true)

# add the binary tree to the search path for include files
# so that we will find rabitConfig.h
include_directories("${PROJECT_BINARY_DIR}")


if(WITHCOVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimization
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage
endif()
#************************************************************************************
# Include some macros to make CMakeLists files cleaner
#************************************************************************************
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

# Make sure we are using the C++11 standard
use_cxx11() # [macro within cmake/utils.cmake]

# Setup Google Testing for unit testing
if(WITHTESTS)
  setup_gtest() # [macro within cmake/utils.cmake]
  enable_testing()
endif()

#************************************************************************************
# The subdirectories
#************************************************************************************
add_subdirectory(rabit) 

if(WITHEXAMPLES)
  add_subdirectory(examples)
endif()

if(WITHTESTS)
  add_subdirectory(test)
endif()


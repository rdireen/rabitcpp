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
set(rabit_VERSION_PATCH 0)
set(VERSION "${rabit_VERSION_MAJOR}.${rabit_VERSION_MINOR}.${rabit_VERSION_PATCH}")


# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/rabitConfig.h.in"
  "${PROJECT_BINARY_DIR}/rabitConfig.h"
  )
  
# add the binary tree to the search path for include files
# so that we will find rabitConfig.h
include_directories("${PROJECT_BINARY_DIR}")




#************************************************************************************
# Include some macros to make CMakeLists files cleaner
#************************************************************************************
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

# Make sure we are using the C++11 standard
use_cxx11() # [macro within cmake/utils.cmake]

# Setup Google Testing for unit testing
setup_gtest() # [macro within cmake/utils.cmake]
enable_testing()

#************************************************************************************
# The subdirectories
#************************************************************************************
add_subdirectory(rabit) 
add_subdirectory(examples)
add_subdirectory(test)

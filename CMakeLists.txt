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
# Make sure we are compiling with C++11
#************************************************************************************

# I will likely have to update this for windows and other operating systems
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
    # Might need to add something in here if we are using other operating systems.
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

use_cxx11()

#************************************************************************************
# Download gmock and gtest for unit testing 
#************************************************************************************


# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleMock
ExternalProject_Add(
    gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)

# Set gmock properties
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)
include_directories("${source_dir}/include")
include_directories(${source_dir}/gtest/include)

enable_testing()

#message("Source Directory For GMOCK GTEST: ${source_dir}" )

#************************************************************************************
# The subdirectories
#************************************************************************************

add_subdirectory(rabit) 
add_subdirectory(tests)
add_subdirectory(test)

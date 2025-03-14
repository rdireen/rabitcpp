#************************************************************************************
# DireenTech Inc.:  www.DireenTech.com
# Harry Direen, PhD, PE:  hdireen@direentech.com
# Randal Direen, PhD: rdireen@direentech.com
# Started April, 2016
#
# Build system for Rabit
#
#************************************************************************************
cmake_minimum_required (VERSION 3.22 FATAL_ERROR)

#************************************************************************************
# Setup vcpkg CMAKE tool chain.  (https://learn.microsoft.com/en-us/vcpkg/get_started/overview)
# Setting the CMAKE_TOOLCHAIN_FILE typically must be done before the 
# CMAKE "project" derective.
#
# This relies on the user having set the enviroment variable VCPKG_ROOT to 
# the installation directory of vcpkg.  The user may also need to add the 
# vcpkg installation directory to their path.
# 
# Note: CMakePresets.json or CMakeUserPresets.json may be used to establish
# the path to vcpkg, but this will change from user to user and may be harder
# to keep track of.
#************************************************************************************

if(WIN32 OR WIN64 OR x64)
    #This needs to be added before the project definition
    file(TO_CMAKE_PATH $ENV{VCPKG_ROOT} VCPKG_ROOT_PATH)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT_PATH}/scripts/buildsystems/vcpkg.cmake")
    message("vcpkg toolchain location: ${CMAKE_TOOLCHAIN_FILE}")

    #Add a flag that can be used in rabit to let rabit know this is a 
    #Windows build.  Otherwise the build is assumed to be Linux.
    add_compile_definitions(WINDOWS=1)

    #Compile for Windows 10 and above.
    add_definitions( -D_WIN32_WINNT=0x0A00 )

endif ()

project (rabit_dev VERSION 2.0.1 LANGUAGES CXX)

#************************************************************************************
# Setup Versioning
#************************************************************************************

# The version number.
set(rabit_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")


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

if(WITHCOVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimization
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage
endif()
#************************************************************************************
# Include some macros to make CMakeLists files cleaner
#************************************************************************************
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup Google Testing for unit testing
if(WITHTESTS)
  setup_gtest() # [macro within cmake/utils.cmake]
  enable_testing()
endif()

#Libraries required by Rabit and programs that use Rabit

#Note: boost is required by Rabit 
find_package ( Boost REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})


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





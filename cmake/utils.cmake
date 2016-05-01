
# ----------------------------------------------------------------------------
# run this to make sure we are using C++11
# ----------------------------------------------------------------------------
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

# ----------------------------------------------------------------------------
# run this to find include directories for the rabit library
# ----------------------------------------------------------------------------
macro(get_include_dirs)
  set(${PROJECT_NAME}_include_dirs "")
  foreach (_headerFile ${${PROJECT_NAME}_headers})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND ${PROJECT_NAME}_include_dirs ${_dir})
  endforeach()
  list(REMOVE_DUPLICATES ${PROJECT_NAME}_include_dirs)
endmacro(get_include_dirs)

# ----------------------------------------------------------------------------
# run this to setup gtest
# ----------------------------------------------------------------------------
macro(setup_gtest)
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

endmacro(setup_gtest)
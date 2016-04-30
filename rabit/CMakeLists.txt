project(rabit)

file(GLOB_RECURSE ${PROJECT_NAME}_headers "src/*.h")
file(GLOB_RECURSE ${PROJECT_NAME}_sources "src/*.cpp")

set(${PROJECT_NAME}_include_dirs "")
foreach (_headerFile ${${PROJECT_NAME}_headers})
  get_filename_component(_dir ${_headerFile} PATH)
  list(APPEND ${PROJECT_NAME}_include_dirs ${_dir})
endforeach()
list(REMOVE_DUPLICATES ${PROJECT_NAME}_include_dirs)

#message("Show the headers: ${${PROJECT_NAME}_headers}")

find_package( Threads )
include_directories(${${PROJECT_NAME}_include_dirs})
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})
target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )

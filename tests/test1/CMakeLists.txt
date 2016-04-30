
project(test1)

file(GLOB_RECURSE ${PROJECT_NAME}_headers "../../rabit/src/*.h")
file(GLOB_RECURSE ${PROJECT_NAME}_sources "../../rabit/src/*.cpp")

set(${PROJECT_NAME}_include_dirs "")
foreach (_headerFile ${${PROJECT_NAME}_headers})
  get_filename_component(_dir ${_headerFile} PATH)
  list(APPEND ${PROJECT_NAME}_include_dirs ${_dir})
endforeach()
list(REMOVE_DUPLICATES ${PROJECT_NAME}_include_dirs)


#message("Include directories: ${${PROJECT_NAME}_include_dirs}")
#message("Headers: ${${PROJECT_NAME}_headers}")
#message("Sources: ${${PROJECT_NAME}_sources}")

set(${PROJECT_NAME}_headers "")
set(${PROJECT_NAME}_sources main.cpp)

set(EXECUTABLE_OUTPUT_PATH "../../")

include_directories(${${PROJECT_NAME}_include_dirs})
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_headers}
                               ${${PROJECT_NAME}_sources})
                               
target_link_libraries(${PROJECT_NAME} rabit)
project(example1)

file(GLOB_RECURSE ${PROJECT_NAME}_headers "../../rabit/src/*.h")
# This macro gets all the include directories from
# ${PROJECT_NAME}_headers and puts them into ${PROJECT_NAME}_include_dirs
get_include_dirs() # within cmake/utils.cmake

set(${PROJECT_NAME}_headers "")
set(${PROJECT_NAME}_sources main.cpp)

set(EXECUTABLE_OUTPUT_PATH "../../")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package( Threads REQUIRED)
include_directories(${${PROJECT_NAME}_include_dirs})
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})
                               
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} rabit)

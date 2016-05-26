project(testutils)

file(GLOB_RECURSE ${PROJECT_NAME}_headers "../../rabit/src/*.h")
# This macro gets all the include directories from
# ${PROJECT_NAME}_headers and puts them into ${PROJECT_NAME}_include_dirs
get_include_dirs() # within cmake/utils.cmake

file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0") # debug, no optimisation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage") # enabling coverage

include_directories(${${PROJECT_NAME}_include_dirs})
add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})
target_link_libraries(${PROJECT_NAME} rabit gcov libgmock)
add_test(${PROJECT_NAME} ${PROJECT_NAME})


#install(TARGETS testfoo DESTINATION bin)

project(testmessaging)

file(GLOB_RECURSE ${PROJECT_NAME}_headers "../../rabit/src/*.h")
# This macro gets all the include directories from
# ${PROJECT_NAME}_headers and puts them into ${PROJECT_NAME}_include_dirs
get_include_dirs() # within cmake/utils.cmake

file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

include_directories(${${PROJECT_NAME}_include_dirs})
add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})
target_link_libraries(${PROJECT_NAME} rabit libgmock)
add_test(${PROJECT_NAME} ${PROJECT_NAME})


#install(TARGETS testfoo DESTINATION bin)

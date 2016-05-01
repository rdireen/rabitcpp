project(testrabit)

file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

add_executable(${PROJECT_NAME} ${HDRS} ${SRCS})
target_link_libraries(${PROJECT_NAME} rabit libgmock)
add_test(${PROJECT_NAME} ${PROJECT_NAME})


#install(TARGETS testfoo DESTINATION bin)

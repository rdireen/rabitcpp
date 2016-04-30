file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)

message(" What ${SRCS}")


add_executable(testrabit ${HDRS} ${SRCS})

target_link_libraries(testrabit
    rabit
    libgmock
)

add_test(testrabit testrabit)


#install(TARGETS testfoo DESTINATION bin)

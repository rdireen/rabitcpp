project(rabit)

find_package (Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE ${PROJECT_NAME}_headers "src/*.h")
file(GLOB_RECURSE ${PROJECT_NAME}_sources "src/*.cpp")

# This macro gets all the include directories from
# ${PROJECT_NAME}_headers and puts them into ${PROJECT_NAME}_include_dirs
get_include_dirs() # within cmake/utils.cmake

find_package( Threads )
include_directories(${${PROJECT_NAME}_include_dirs})
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})
target_link_libraries( ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )


set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${rabit_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${rabit_VERSION_MAJOR} )
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${${PROJECT_NAME}_headers} DESTINATION include/rabit)

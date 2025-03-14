project(rabit)

find_package ( Boost REQUIRED )
find_package( Boost COMPONENTS system filesystem REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})
message("[rcs] Boost Include directory(s): ${Boost_INCLUDE_DIRS}")
message("[rcs] Boost Library directory(s): ${Boost_LIBRARIES}")

set(RABIT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#==============================================================================
#                  Add Individual Sources for Rabit Here
#
# This was done using GLOB before, but this way is better. By explicitly 
# listing the files they will be tracked by CMake and IDEs.
set(RABIT_H
    ${RABIT_SRC}/Manager/RabitManager.h
    ${RABIT_SRC}/Messaging/ManagerControlMessage.h
    ${RABIT_SRC}/Messaging/ManagerStatusMessage.h
    ${RABIT_SRC}/Messaging/PublishSubscribeMessage.h
    ${RABIT_SRC}/Messaging/PublishSubscribeMsgArray.h
    ${RABIT_SRC}/Messaging/RabitMessage.h
    ${RABIT_SRC}/Messaging/RabitMessageQueue.h
    ${RABIT_SRC}/Messaging/RabitNonBlockingSPSCQueue.h
    ${RABIT_SRC}/Messaging/RabitWorkspace.h
    ${RABIT_SRC}/Messaging/message_pool.h
    ${RABIT_SRC}/RabitConfig.h 
    ${RABIT_SRC}/RabitReactor.h 
    ${RABIT_SRC}/StateSystem/ConstantStateNode.h
    ${RABIT_SRC}/StateSystem/CounterStateNode.h
    ${RABIT_SRC}/StateSystem/MasterStateNode.h
    ${RABIT_SRC}/StateSystem/StateNode.h
    ${RABIT_SRC}/StateSystem/TimerDelayStateNode.h
    ${RABIT_SRC}/Utilities/Base64Conversions.h
    ${RABIT_SRC}/Utilities/ByteArrayReaderWriter.h
    ${RABIT_SRC}/Utilities/CRC_Calculator.h
    ${RABIT_SRC}/Utilities/GlobalPublishSubscribeException.h
    ${RABIT_SRC}/Utilities/ManagerStats.h
    ${RABIT_SRC}/Utilities/MessageNotRegisteredException.h
    ${RABIT_SRC}/Utilities/RabitStopWatch.h
    ${RABIT_SRC}/Utilities/SafeQueue.h
    ${RABIT_SRC}/Utilities/SystemTimeClock.h
    )

set(RABIT_CPP
    ${RABIT_SRC}/Manager/RabitManager.cpp
    ${RABIT_SRC}/Messaging/ManagerControlMessage.cpp
    ${RABIT_SRC}/Messaging/ManagerStatusMessage.cpp
    ${RABIT_SRC}/Messaging/PublishSubscribeMessage.cpp
    ${RABIT_SRC}/Messaging/PublishSubscribeMsgArray.cpp
    ${RABIT_SRC}/Messaging/RabitMessage.cpp
    ${RABIT_SRC}/Messaging/RabitMessageQueue.h
    ${RABIT_SRC}/Messaging/RabitNonBlockingSPSCQueue.h
    ${RABIT_SRC}/Messaging/RabitWorkspace.cpp
    ${RABIT_SRC}/Messaging/message_pool.cpp
    ${RABIT_SRC}/RabitReactor.cpp
    ${RABIT_SRC}/StateSystem/CounterStateNode.cpp
    ${RABIT_SRC}/StateSystem/MasterStateNode.cpp
    ${RABIT_SRC}/StateSystem/TimerDelayStateNode.cpp
    ${RABIT_SRC}/Utilities/Base64Conversions.cpp
    ${RABIT_SRC}/Utilities/ByteArrayReaderWriter.cpp
    ${RABIT_SRC}/Utilities/CRC_Calculator.cpp
    ${RABIT_SRC}/Utilities/ManagerStats.cpp
    ${RABIT_SRC}/Utilities/RabitStopWatch.cpp
    ${RABIT_SRC}/Utilities/SystemTimeClock.cpp
)

file(COPY ${RABIT_H} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)

# Get the included directories from the list of *.h files and put them in
# ${PROJECT_NAME}_include_dirs
get_directories_from_file_list("${RABIT_H}") 

find_package( Threads )

#add_library(${PROJECT_NAME} SHARED  ${RABIT_CPP})
add_library(${PROJECT_NAME} STATIC  ${RABIT_CPP} ${RABIT_H})

target_include_directories(${PROJECT_NAME}  PRIVATE                          
                        ${PROJECT_BINARY_DIR} 
                        ${${PROJECT_NAME}_include_dirs}
                        ${Boost_INCLUDE_DIRS}
)

target_include_directories(${PROJECT_NAME} INTERFACE
                        "$<BUILD_INTERFACE:${${PROJECT_NAME}_include_dirs}>"
                        "$<INSTALL_INTERFACE:include/rabit>"
)

target_link_libraries( ${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${rabit_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${rabit_VERSION_MAJOR} )

install(TARGETS ${PROJECT_NAME} 
        DESTINATION lib
        EXPORT RabitTargets 
        )

install(FILES ${RABIT_H} 
        DESTINATION include/rabit)

install(EXPORT RabitTargets
  FILE RabitTargets.cmake
  DESTINATION lib/cmake/Rabit
)

include(CMakePackageConfigHelpers)
# generate the config file that includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/RabitConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/example"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/RabitConfigVersion.cmake"
  VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/RabitConfig.cmake
  DESTINATION lib/cmake/rabit
  )

  export(EXPORT RabitTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/RabitTargets.cmake"
)

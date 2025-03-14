

include(px4_parse_function_args)



function(rabit_executable)

    px4_parse_function_args(
        NAME rabit_add_messages
        ONE_VALUE TARGET_NAME RABIT_CONFIGURATION MANAGER_LOCATION MAIN_TEMPLATE MAIN_TEMPLATE_DIR
        MULTI_VALUE INCLUDE_DIRECTORIES LIBRARIES
        REQUIRED TARGET_NAME RABIT_CONFIGURATION MANAGER_LOCATION MAIN_TEMPLATE
        ARGN ${ARGN})

    set(OUTPUT_MAIN main.cpp)
    set(OUTPUT_MAIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/main_out)
    set(NAMESPACE xkcd)

    include(${RABIT_CONFIGURATION})

    message("[rabit] Adding Managers:")
    foreach(manager ${RABIT_SYSTEM_MANAGERS})
        message("[rabit] - Adding ${manager} From Configuration File")
        add_subdirectory(${MANAGER_LOCATION}/${manager})
    endforeach()

    message("[rabit] TEMPLATE ${MAIN_TEMPLATE}")
    # Generate message headers in the make file, this depends on msg_headers
    add_custom_command(OUTPUT ${OUTPUT_MAIN_DIR}/${OUTPUT_MAIN}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_main_gen.py
                -p ${MAIN_TEMPLATE}
                -i ${MAIN_TEMPLATE_DIR}
                -m ${RABIT_SYSTEM_MANAGERS}
                -n ${NAMESPACE}
                -o ${OUTPUT_MAIN_DIR}
                ${added_arguments}
            DEPENDS
                ${MAIN_TEMPLATE}
                ${RBX_TOOLS}/rabit_main_gen.py
            COMMENT "[rabit] Generating project main.cpp"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )

    add_custom_target(main_gen DEPENDS ${OUTPUT_MAIN_DIR}/${OUTPUT_MAIN})

    add_executable(${TARGET_NAME} )
    add_dependencies(${TARGET_NAME} main_gen)
    target_sources(${TARGET_NAME} PRIVATE ${OUTPUT_MAIN_DIR}/${OUTPUT_MAIN} )

    target_include_directories(${TARGET_NAME} 
                            PRIVATE 
                                ${INCLUDE_DIRECTORIES}
                                )

    target_link_libraries( ${TARGET_NAME}
                            PRIVATE
                                rabit
                                rabit_msgs
                                ${LIBRARIES}
                                ${RABIT_SYSTEM_MANAGERS}
                                )
 

endfunction()
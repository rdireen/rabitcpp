

# cmake include guard
if(rabit_add_manager_included)
	return()
endif(rabit_add_manager_included)
set(rabit_add_manager_included true)

include(px4_parse_function_args)

function(rabit_add_manager)

    px4_parse_function_args(
        NAME rabit_add_manager
        ONE_VALUE MANAGER MESSAGES_CONFIG WORKSPACE_H_TEMPLATE WORKSPACE_CPP_TEMPLATE NAMESPACE
        MULTI_VALUE INCLUDES SRCS DEPENDS
        REQUIRED MANAGER INCLUDES SRCS MESSAGES_CONFIG WORKSPACE_H_TEMPLATE WORKSPACE_CPP_TEMPLATE NAMESPACE
        ARGN ${ARGN})

    message("[rabit] -- Adding manager ${MANAGER} to build")

    set(RABIT_MNG_WORKSPACES ${CMAKE_BINARY_DIR}/rabit_manager_workspaces)
    set(MNG_WOKSPACE_HEADER ${MANAGER}WSRMgr.h)
    set(MNG_WORKSPACE_SOURCE ${MANAGER}WSRMgr.cpp)
    set(MESSAGES_CONFIG_YML ${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGES_CONFIG})
    set(TEMPLATE_H ${WORKSPACE_H_TEMPLATE})
    set(TEMPLATE_CPP ${WORKSPACE_CPP_TEMPLATE})

    set(WS_HEADER ${RABIT_MNG_WORKSPACES}/${MNG_WOKSPACE_HEADER})
    set(WS_SOURCE ${RABIT_MNG_WORKSPACES}/${MNG_WORKSPACE_SOURCE})

    add_custom_command(OUTPUT ${WS_SOURCE} ${WS_HEADER}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_manager_workspace_gen.py
                -i ${MNG_WOKSPACE_HEADER}
                -c ${MNG_WORKSPACE_SOURCE}
                -o ${RABIT_MNG_WORKSPACES}
                -p ${TEMPLATE_H}
                -q ${TEMPLATE_CPP}
                -y ${MESSAGES_CONFIG_YML}
                -m ${RABIT_SYSTEM_MESSAGES}
                -n ${NAMESPACE}
                ${added_arguments}
            DEPENDS
                ${MESSAGES_CONFIG_YML}
                ${TEMPLATE_CPP}
                ${TEMPLATE_H}
                ${RBX_TOOLS}/rabit_manager_workspace_gen.py
            COMMENT "[rabit] Generating rabit workspace source"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )
    add_custom_target(${MANAGER}mng_headers DEPENDS ${WS_HEADER})

    get_target_property(RABIT_INCLUDE_DIR rabit INTERFACE_INCLUDE_DIRECTORIES)

    add_library(${MANAGER} STATIC ${SRCS} ${INCLUDES} ${WS_SOURCE} ${WS_HEADER})
    add_dependencies(${MANAGER} ${MANAGER}mng_headers)

    target_link_libraries(${MANAGER}  
                                PRIVATE
                                    rabit 
                                    rabit_msgs
                                    ${DEPENDS}
                                    )

    target_include_directories(${MANAGER} 
                                    PRIVATE 
                                        ${RABIT_INCLUDE_DIR}
                                        ${MANAGER_INCLUDE_DIRECTORIES}
                                        ${RABIT_MNG_WORKSPACES}
                                        )

    target_include_directories(${MANAGER}  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${RABIT_MNG_WORKSPACES})

endfunction()
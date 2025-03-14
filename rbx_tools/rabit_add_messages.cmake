
# cmake include guard
if(rabit_add_messages_included)
	return()
endif(rabit_add_messages_included)
set(rabit_add_messages_included true)

include(px4_parse_function_args)

function(rabit_add_messages)

    px4_parse_function_args(
        NAME rabit_add_messages
        ONE_VALUE NAMESPACE H_TEMPLATE CPP_TEMPLATE CSHP_TEMPLATE CSHP_OUTDIRECTORY CS_NAMESPACE PYTHON_TEMPLATE PYTHON_OUTDIRECTORY
        MULTI_VALUE MESSAGES DEPRICATED
        REQUIRED NAMESPACE MESSAGES H_TEMPLATE CPP_TEMPLATE
        ARGN ${ARGN})

    set(msg_files ${MESSAGES})

    # This section may be useful when we want to gracefully get rid of messages
    foreach(msg IN LISTS ${DEPRICATED})
        if(msg IN_LIST msg_files)
            get_filename_component(msg_we ${msg} NAME_WE)
            list(APPEND invalid_msgs ${msg_we})
        endif()
    endforeach()
    if(invalid_msgs)
        list(LENGTH invalid_msgs invalid_msgs_size)
        if(${invalid_msgs_size} GREATER 1)
            foreach(msg IN LISTS invalid_msgs)
                string(CONCAT invalid_msgs_cs ${invalid_msgs_cs} "'${msg}', ")
            endforeach()
            STRING(REGEX REPLACE ", +$" "" invalid_msgs_cs ${invalid_msgs_cs})
            message(FATAL_ERROR "[rabit] ${invalid_msgs_cs} are listed as deprecated. Please use different names for the messages.")
        else()
            message(FATAL_ERROR "[rabit] '${invalid_msgs}' is listed as deprecated. Please use a different name for the message.")
        endif()
    endif()

    if(NOT DEFINED CSHP_OUTDIRECTORY)
        set(CSHP_OUTDIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/csharp_msgs)
    endif()

    if(NOT DEFINED PYTHON_OUTDIRECTORY)
        set(PYTHON_OUTDIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python_msgs)
    endif()

    set(RABIT_MSG_INCLUDE_DIR ${CMAKE_BINARY_DIR}/rabit_message_headers)
    set(msg_source_out_path	${CMAKE_CURRENT_BINARY_DIR}/tmp/sources)
    set(msg_headers "")
    set(msg_sources "")
    set(msg_csharps "")
    set(msg_python "")
    set(RABIT_SYSTEM_MESSAGES "")
    foreach(msg_file ${msg_files})
        get_filename_component(msg ${msg_file} NAME_WE)
        list(APPEND msg_headers ${msg_out_path}/${msg}.h)
        list(APPEND msg_sources ${msg_source_out_path}/${msg}.cpp)
        list(APPEND msg_csharps ${CSHP_OUTDIRECTORY}/${msg}.cs)
        list(APPEND msg_python ${PYTHON_OUTDIRECTORY}/${msg}.py)
        list(APPEND RABIT_SYSTEM_MESSAGES ${msg}) 
    endforeach()

    set(RABIT_SYSTEM_MESSAGES ${RABIT_SYSTEM_MESSAGES} PARENT_SCOPE)

    if(DEFINED CSHP_TEMPLATE)
        message("[rabit] Setting up CSharp generated messages at ${CSHP_OUTDIRECTORY}")
        if(NOT DEFINED CS_NAMESPACE)
            set(CS_NAMESPACE " ")
        endif() 

        add_custom_command(OUTPUT ${msg_csharps}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_msg_gen.py
                --csharps
                -f ${msg_files}
                -i ${CMAKE_CURRENT_SOURCE_DIR}
                -o ${CSHP_OUTDIRECTORY}
                -e templates
                -p ${CSHP_TEMPLATE}
                -t ${CMAKE_CURRENT_BINARY_DIR}/tmp/csharp
                -n ${CS_NAMESPACE}
                ${added_arguments}
            DEPENDS
                ${msg_files}
                templates/${CSHP_TEMPLATE}
                ${RBX_TOOLS}/rabit_msg_gen.py
            COMMENT "[rabit] Generating rabit message cs files"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )
        add_custom_target(msg_csharps DEPENDS ${msg_csharps})

    endif()

    if(DEFINED PYTHON_TEMPLATE)
        message("[rabit] Setting up Python generated messages at ${PYTHON_OUTDIRECTORY}")
        if(NOT DEFINED PY_NAMESPACE)
            set(PY_NAMESPACE " ")
        endif() 

        add_custom_command(OUTPUT ${msg_python}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_msg_gen.py
                --python
                -f ${msg_files}
                -i ${CMAKE_CURRENT_SOURCE_DIR}
                -o ${PYTHON_OUTDIRECTORY}
                -e templates
                -p ${PYTHON_TEMPLATE}
                -t ${CMAKE_CURRENT_BINARY_DIR}/tmp/python
                -n ${PY_NAMESPACE}
                ${added_arguments}
            DEPENDS
                ${msg_files}
                templates/${PYTHON_TEMPLATE}
                ${RBX_TOOLS}/rabit_msg_gen.py
            COMMENT "[rabit] Generating rabit message py files"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )
        add_custom_target(msg_python DEPENDS ${msg_python})

    endif()
    
    # Generate message headers in the make file, this depends on msg_headers
    add_custom_command(OUTPUT ${msg_headers}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_msg_gen.py
                --headers
                -f ${msg_files}
                -i ${CMAKE_CURRENT_SOURCE_DIR}
                -o ${RABIT_MSG_INCLUDE_DIR}
                -e templates
                -p ${H_TEMPLATE}
                -t ${CMAKE_CURRENT_BINARY_DIR}/tmp/headers
                -n ${NAMESPACE}
                ${added_arguments}
            DEPENDS
                ${msg_files}
                templates/${H_TEMPLATE}
                ${RBX_TOOLS}/rabit_msg_gen.py
            COMMENT "[rabit] Generating rabit message h files"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )
    add_custom_target(msg_headers DEPENDS ${msg_headers})

    # Generate message headers in the make file, this depends on msg_headers
    add_custom_command(OUTPUT ${msg_sources}
            COMMAND ${PYTHON_EXECUTABLE} ${RBX_TOOLS}/rabit_msg_gen.py
                --sources
                -f ${msg_files}
                -i ${CMAKE_CURRENT_SOURCE_DIR}
                -o ${RABIT_MSG_INCLUDE_DIR}
                -e templates
                -p ${CPP_TEMPLATE}
                -t ${CMAKE_CURRENT_BINARY_DIR}/tmp/sources
                -n ${NAMESPACE}
                ${added_arguments}
            DEPENDS
                ${msg_files}
                templates/${CPP_TEMPLATE}
                ${RBX_TOOLS}/rabit_msg_gen.py
            COMMENT "[rabit] Generating rabit message cpp files"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
            )

    # I'm not sure if this is a hack or not, but I needed these include directories
    get_target_property(RABIT_INCLUDE_DIR rabit INTERFACE_INCLUDE_DIRECTORIES)

    add_library(rabit_msgs STATIC ${msg_sources})
    add_dependencies(rabit_msgs rabit msg_headers msg_csharps msg_python)
    target_include_directories(rabit_msgs PRIVATE ${RABIT_MSG_INCLUDE_DIR} ${RABIT_INCLUDE_DIR})
    target_include_directories(rabit_msgs INTERFACE ${RABIT_MSG_INCLUDE_DIR})

    set_property(TARGET rabit_msgs PROPERTY VERSION 0)
    set_property(TARGET rabit_msgs PROPERTY SOVERSION 1 )

endfunction()
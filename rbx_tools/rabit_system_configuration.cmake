# cmake include guard
if(rabit_system_configuration_included)
	return()
endif(rabit_system_configuration_included)
set(rabit_system_configuration_included true)

include(px4_parse_function_args)



function(rabit_system_configuration)

    px4_parse_function_args(
        NAME rabit_add_system_configuration
        ONE_VALUE CONFIG_NAME
        MULTI_VALUE MANAGERS
        REQUIRED MANAGERS
        ARGN ${ARGN})

    set(RABIT_SYSTEM_NAME ${CONFIG_NAME} PARENT_SCOPE)
    set(RABIT_SYSTEM_MANAGERS ${MANAGERS} PARENT_SCOPE)

endfunction()
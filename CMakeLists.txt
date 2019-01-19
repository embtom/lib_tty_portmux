project(lib_ttyportmux)

#######################################################################################
#Generation of a Interface Header 
#depending of the source list
#######################################################################################
FUNCTION(GENERATE_TTY_PORT_PLUGIN_IF _PLUGIN_SRC_LIST)

	SET(PLUGIN_SRC_LIST ${_PLUGIN_SRC_LIST})
	
	#Get number of available plugins
	LIST(LENGTH PLUGIN_SRC_LIST GEN_HEADER_PLUGIN_COUNT)
	
	#Write Header include
	FOREACH(item ${PLUGIN_SRC_LIST})
		message(STATUS "+++ S${item}")
		get_filename_component(item ${item} NAME)
		STRING(REGEX MATCH "tty_port[a-zA-Z0-9_]*." item ${item})
		message(STATUS "+++ S${item}")
  		IF(item)
    		STRING(APPEND GEN_HEADER_INCLUDES "#include <${item}h>\n")
    		STRING(APPEND GEN_IF_INIT_CALLS "\t${item}__share_if();\n") 		
  		ENDIF(item)
	ENDFOREACH(item ${_PLUGIN_SRC_LIST})
	#string (REPLACE ";" "" GEN_HEADER_INCLUDES "${GEN_HEADER_INCLUDES}")
	string (REPLACE "." "" GEN_IF_INIT_CALLS "${GEN_IF_INIT_CALLS}")
	
	#Write Config init file
	configure_file(${PROJECT_SRC_DIR}/tty_portplugin_init.h.in ${PROJECT_BINARY_DIR}/tty_portplugin_init.h)
ENDFUNCTION()

#######################################################################################
#Build Settings 
#######################################################################################
SET(PROJECT_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
SET(PROJECT_PLUGIN_DIR ${PROJECT_SOURCE_DIR}/plugins)
SET(PROJECT_LINK_LIBRARIES lib_convention )
SET(SOURCES ${PROJECT_SRC_DIR}/lib_ttyportmux.c)

#######################################################################################
#Check envirionment setup environment variables
#######################################################################################

LIST(APPEND PROJECT_LINK_LIBRARIES lib_list lib_mini_printf)

#######################################################################################
#Check plugins to load
#######################################################################################
#List of available plugins
SET(PROJECT_PLUGINS "unix" "syslog" "console" "trace_CORTEXM")
if (UNIX)
	SET(PROJECT_DEFINES _GNU_SOURCE)
	# At unix os add unix port
	LIST(APPEND SOURCES_PLUGIN "${PROJECT_PLUGIN_DIR}/tty_portunix.c")
	LIST(APPEND SOURCES_PLUGIN "${PROJECT_PLUGIN_DIR}/tty_portsyslog.c")
endif()

#Only plugins are installed if the corresponding driver target exits
foreach(var ${PROJECT_PLUGINS})
	SET(PLUGIN_TARGET_NAME "lib_${var}")
	if(TARGET ${PLUGIN_TARGET_NAME})
		LIST(APPEND SOURCES_PLUGIN "${PROJECT_PLUGIN_DIR}/tty_port${var}.c")
		LIST(APPEND PROJECT_LINK_LIBRARIES "${PLUGIN_TARGET_NAME}")	
	endif()
endforeach()

GENERATE_TTY_PORT_PLUGIN_IF("${SOURCES_PLUGIN}")

foreach(ITEM ${SOURCES_PLUGIN})
	get_filename_component(pluginName ${ITEM} NAME)
	message(STATUS "${PROJECT_NAME} - Plugin ${pluginName}")
endforeach()

#######################################################################################
#Build target settings
#######################################################################################
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${SOURCES_PLUGIN})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ./include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SRC_DIR} ${PROJECT_PLUGIN_DIR} ${PROJECT_BINARY_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_DEFINES})




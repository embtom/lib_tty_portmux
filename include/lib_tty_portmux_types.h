/* ****************************************************************************************************
 * lib_tty_portmux_types.h within the following project: bld_device_cmake_Nucleo_STM32F401_
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	Aug 29, 2018			thomas			- creation of lib_tty_portmux_types.h
 *  
 */



#ifndef _LIB_TTY_PORTMUX_TYPES_H_
#define _LIB_TTY_PORTMUX_TYPES_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_STREAM_MAPPING_ENTRY(__port_type) \
{													  \
	.device_type = __port_type,						  \
	.port_device = NULL								  \
}

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
enum tty_stream {
	TTY_STREAM_critical,
	TTY_STREAM_error,
	TTY_STREAM_warning,
	TTY_STREAM_info,
	TTY_STREAM_debug,
	TTY_STREAM_CONTROL,
	TTY_STREAM_CNT
};

enum tty_device_type {
	TTY_DEVICE_console,
	TTY_DEVICE_trace_CORTEXM,
	TTY_DEVICE_unix,
	TTY_DEVICE_syslog
};


struct stream_info {
	enum tty_stream stream;
	char *stream_name;
	enum tty_device_type device_type;
	char *device_type_name;
	unsigned int device_index;
};

struct device_info {
	enum tty_device_type device_type;
	char *device_type_name;
	unsigned int device_index;
	unsigned int device_number;
};


typedef struct tty_device tty_device_t;
struct tty_stream_mapping
{
	enum tty_device_type device_type;
 	tty_device_t *port_device;
};

#endif /* _LIB_TTY_PORTMUX_TYPES_H_ */

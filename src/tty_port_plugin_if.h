/* ****************************************************************************************************
 * lib_port_mux.h for multiplexing of tty ports
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	15 August 2018	Tom				- creation of lib_tty_portmux.h
 *
 */

#ifndef INT_IO_TTY_LIB_TTY_PORTMUX_SH_LIB_TTY_PORTMUX_INC_INT_PORT_IF_PLUGIN_H_
#define INT_IO_TTY_LIB_TTY_PORTMUX_SH_LIB_TTY_PORTMUX_INC_INT_PORT_IF_PLUGIN_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c runtime */
#include <stdarg.h>
#include <stddef.h>
/* frame */
#include <lib_list_types.h>
/*project */
#include "lib_tty_portmux_types.h"

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
struct tty_device
{
	struct list_node node;
	void *port_hdl;
	struct tty_driver *port_driver;		/*driver structure passed during init*/
};

typedef int (tty_open_t)(tty_device_t *_tty_device);
typedef int (tty_close_t)(tty_device_t *_tty_device);
typedef int (tty_write_t)(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap);
typedef int (tty_put_char)(tty_device_t *_tty_device, enum tty_stream _stream, char _c);
typedef int (tty_read_t)(tty_device_t *_tty_device, enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter);

/* ************************************************************************//**
 * \brief	actual structure which is used for exchanging the function
 * 			pointers.
 * ****************************************************************************/
typedef struct tty_driver {
	struct device_info info;
	tty_open_t *open;				/*!< initialization function */
	tty_close_t *close;			/*!< cleanup function */
	tty_write_t *write;				/*!<  */
	tty_put_char *put_char;		/*!< seek function */
	tty_read_t *read;
	tty_device_t *device;
} tty_driver_t;


/* ************************************************************************//**
 * \brief	No Interface function only for internal -  Register a stdio channel at the port multiplexer
 *
 * \param   _port_driver  share interfaces with the port multiplexer
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int tty_port_register(tty_driver_t * const _port_if);

#endif /* INT_IO_TTY_LIB_TTY_PORTMUX_SH_LIB_TTY_PORTMUX_INC_INT_PORT_IF_PLUGIN_H_ */

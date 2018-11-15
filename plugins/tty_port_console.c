/* ****************************************************************************************************
 * tty_port_console.c within the following project: bld_device_cmake_Nucleo_STM32F401
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	16.08.2018			thomas			- creation of tty_port_trace_CORTEXM.c
 *  
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c -runtime */
#include <stdarg.h>

/* frame */
#include <lib_convention__errno.h>
#include <lib_console.h>

/* project */
#include <lib_tty_portmux_types.h>
#include "tty_port_console.h"

/* *******************************************************************
 * defines
 * ******************************************************************/


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_console__open(tty_device_t *_tty_device);
static int tty_port_console__close(tty_device_t *_tty_device);
static int tty_port_console__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap);
static int tty_port_console__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c);
static int tty_port_console__read (tty_device_t *_tty_device, enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static tty_driver_t s_tty_port_console = {
	.info.device_type_name = "TTY_DEVICE_console",
	.info.device_type = TTY_DEVICE_console,
	.info.device_number = 1,
	.open = &tty_port_console__open,
	.close = &tty_port_console__close,
	.write = &tty_port_console__write,
	.put_char =&tty_port_console__put_char,
	.read = &tty_port_console__read,
	.device = NULL
};


/* *******************************************************************
 * \brief	sharing the interfaces
 * ---------
 * \remark  The execution of a thread stops only at cancellation points
 * ---------
 * \param	_share	[in/out] :	pointer for sharing the interfaces
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int tty_port_console__share_if(void)
{
	int device_number;
	device_number = lib_console__get_device_number();
	if (device_number < EOK) {
		return device_number;
	}
	s_tty_port_console.info.device_number = device_number;


	tty_port_register(&s_tty_port_console);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_console__open(tty_device_t *_tty_device)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__open((console_hdl_t*)&_tty_device->port_hdl,_tty_device->port_driver->info.device_index);
	if (ret_val < EOK) {
		_tty_device->port_hdl = NULL;
	}

	return ret_val;
}

static int tty_port_console__close(tty_device_t *_tty_device)
{
	int ret_val;



	return EOK;
}

static int tty_port_console__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__vprint_debug_message((console_hdl_t)_tty_device->port_hdl, _format, _ap);
	return ret_val;
}

static int tty_port_console__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__putchar((console_hdl_t)_tty_device->port_hdl,_c);
	return ret_val;
}

static int tty_port_console__read (tty_device_t *_tty_device, enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__getdelim((console_hdl_t)_tty_device->port_hdl, _lineptr, _n, _delimiter);
	return ret_val;
}



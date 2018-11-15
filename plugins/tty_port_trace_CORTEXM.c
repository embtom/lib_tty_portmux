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
#include <Trace.h>

/* project */
#include <lib_tty_portmux_types.h>
#include "tty_port_trace_CORTEXM.h"

/* *******************************************************************
 * defines
 * ******************************************************************/


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_trace_CORTEXM__open(tty_device_t *_tty_device);
static int tty_port_trace_CORTEXM__close(tty_device_t *_tty_device);
static int tty_port_trace_CORTEXM__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap);
static int tty_port_trace_CORTEXM__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static tty_driver_t s_tty_port_trace_CORTEXM= {
	.info.device_type_name = "TTY_DEVICE_trace_CORTEXM",
	.info.device_type = TTY_DEVICE_trace_CORTEXM,
	.info.device_number = 1,
	.open = &tty_port_trace_CORTEXM__open,
	.close = &tty_port_trace_CORTEXM__close,
	.write = &tty_port_trace_CORTEXM__write,
	.put_char =&tty_port_trace_CORTEXM__put_char,
	.read = NULL,
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
int tty_port_trace_CORTEXM__share_if(void)
{
	tty_port_register(&s_tty_port_trace_CORTEXM);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_trace_CORTEXM__open(tty_device_t *_tty_device)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__open((trace_swo_hdl_t*)&_tty_device->port_hdl,_tty_device->port_driver->info.device_index);
	if (ret_val < EOK) {
		_tty_device->port_hdl = NULL;
	}

	return ret_val;
}

static int tty_port_trace_CORTEXM__close(tty_device_t *_tty_device)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_trace_CORTEXM__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__vprintf((trace_swo_hdl_t)_tty_device->port_hdl, _format, _ap);

	return ret_val;
}

static int tty_port_trace_CORTEXM__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__putchar((trace_swo_hdl_t)_tty_device->port_hdl, _c);

	return ret_val;
}




/* ****************************************************************************************************
 * tty_port_unix.c within the following project: bld_device_cmake_Nucleo_STM32F401
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
#include <stdio.h>

/* frame */
#include <lib_convention__errno.h>

/* project */
#include <lib_tty_portmux_types.h>
#include "tty_port_unix.h"

/* *******************************************************************
 * defines
 * ******************************************************************/

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_unix__open(tty_device_t *_tty_device);
static int tty_port_unix__close(tty_device_t *_tty_device);
static int tty_port_unix__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap);
static int tty_port_unix__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c);
static int tty_port_unix__read (tty_device_t *_tty_device, enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static tty_driver_t s_tty_port_unix= {
	.info.device_type_name = "TTY_DEVICE_unix",
	.info.device_type = TTY_DEVICE_unix,
	.info.device_number = 1,
	.open = &tty_port_unix__open,
	.close = &tty_port_unix__close,
	.write = &tty_port_unix__write,
	.put_char =&tty_port_unix__put_char,
	.read = &tty_port_unix__read,
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
int tty_port_unix__share_if(void)
{
	tty_port_register(&s_tty_port_unix);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_unix__open(tty_device_t *_tty_device)
{
	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}
	return EOK;
}

static int tty_port_unix__close(tty_device_t *_tty_device)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_unix__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	vprintf(_format, _ap);
	fflush(stdout);
	return EOK;

}

static int tty_port_unix__put_char(tty_device_t *_tty_device, enum tty_stream _stream, char _c)
{
	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	putchar(_c);
	fflush(stdout);
	return EOK;
}

static int tty_port_unix__read (tty_device_t *_tty_device, enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret_val;
	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	if ((_lineptr == NULL) || (_n == NULL)) {
		return -EPAR_NULL;
	}

	if (_delimiter == '\r') {
		_delimiter = '\n';
	}

	ret_val = getdelim(&_lineptr,_n,_delimiter,stdin);
	if (ret_val == -1) {
		ret_val = convert_std_errno(errno);
	}
	_lineptr[ret_val -1] = 0;

	return EOK;
}


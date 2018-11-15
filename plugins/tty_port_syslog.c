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
#include <syslog.h>
#include <errno.h>

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
static int tty_port_syslog__open(tty_device_t *_tty_device);
static int tty_port_syslog__close(tty_device_t *_tty_device);
static int tty_port_syslog__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static tty_driver_t s_tty_port_syslog= {
	.info.device_type_name = "TTY_DEVICE_syslog",
	.info.device_type = TTY_DEVICE_syslog,
	.info.device_number = 1,
	.open = &tty_port_syslog__open,
	.close = &tty_port_syslog__close,
	.write = &tty_port_syslog__write,
	.put_char =NULL,
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
int tty_port_syslog__share_if(void)
{
	tty_port_register(&s_tty_port_syslog);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_syslog__open(tty_device_t *_tty_device)
{
	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	#if defined(__APPLE__) || defined(__FreeBSD__)
		const char * appname = getprogname();
	#elif defined(_GNU_SOURCE)
		const char * appname = program_invocation_short_name;
	#else
		const char * appname = "?";
	#endif

	openlog(appname, LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_USER);
	return EOK;
}

static int tty_port_syslog__close(tty_device_t *_tty_device)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_syslog__write(tty_device_t *_tty_device, enum tty_stream _stream, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_tty_device == NULL) {
		return -ESTD_INVAL;
	}

	switch (_stream) 
	{
		case TTY_STREAM_CONTROL:
			vsyslog(LOG_NOTICE, _format, _ap);
			break;
		case TTY_STREAM_debug:
			vsyslog(LOG_DEBUG, _format, _ap);
			break;
		case TTY_STREAM_info:
			vsyslog(LOG_INFO, _format, _ap);
			break;
		case TTY_STREAM_warning:
			vsyslog(LOG_WARNING, _format, _ap);
			break;
		case TTY_STREAM_error:
			vsyslog(LOG_ERR, _format, _ap);
			break;
		case TTY_STREAM_critical: 
			vsyslog(LOG_CRIT, _format, _ap);
			break;
		default:
			break;
	}	
	return EOK;
}


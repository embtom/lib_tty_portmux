/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/tom3333)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include <lib_ttyportmux_types.h>
#include "tty_portunix.h"

/* *******************************************************************
 * defines
 * ******************************************************************/


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_syslog__open(ttydevice_t *_ttydevice);
static int tty_port_syslog__close(ttydevice_t *_ttydevice);
static int tty_port_syslog__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static ttydriver_t s_ttydriver_syslog= {
	.info.deviceName = "TTYDEVICE_syslog",
	.info.deviceType = TTYDEVICE_syslog,
	.info.deviceNumber = 1,
	.open = &tty_port_syslog__open,
	.close = &tty_port_syslog__close,
	.write = &tty_port_syslog__write,
	.put_char =NULL,
	.read = NULL,
	.ttydevice = NULL
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
int tty_portsyslog__share_if(void)
{
	tty_driver_register(&s_ttydriver_syslog);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_syslog__open(ttydevice_t *_ttydevice)
{
	if (_ttydevice == NULL) {
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

static int tty_port_syslog__close(ttydevice_t *_ttydevice)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_syslog__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	switch (_streamType) 
	{
		case TTYSTREAM_control:
			vsyslog(LOG_NOTICE, _format, _ap);
			break;
		case TTYSTREAM_debug:
			vsyslog(LOG_DEBUG, _format, _ap);
			break;
		case TTYSTREAM_info:
			vsyslog(LOG_INFO, _format, _ap);
			break;
		case TTYSTREAM_warning:
			vsyslog(LOG_WARNING, _format, _ap);
			break;
		case TTYSTREAM_error:
			vsyslog(LOG_ERR, _format, _ap);
			break;
		case TTYSTREAM_critical: 
			vsyslog(LOG_CRIT, _format, _ap);
			break;
		default:
			break;
	}	
	return EOK;
}


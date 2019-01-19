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

/* frame */
#include <lib_convention__errno.h>
#include <Trace.h>

/* project */
#include <lib_ttyportmux_types.h>
#include "tty_porttrace_CORTEXM.h"

/* *******************************************************************
 * defines
 * ******************************************************************/


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_trace_CORTEXM__open(ttydevice_t *_ttydevice);
static int tty_port_trace_CORTEXM__close(ttydevice_t *_ttydevice);
static int tty_port_trace_CORTEXM__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap);
static int tty_port_trace_CORTEXM__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static ttydriver_t s_ttydriver_trace_CORTEXM = {
	.info.deviceName = "TTYDEVICE_trace_CORTEXM",
	.info.deviceType = TTYDEVICE_trace_CORTEXM,
	.info.deviceNumber = 1,
	.open = &tty_port_trace_CORTEXM__open,
	.close = &tty_port_trace_CORTEXM__close,
	.write = &tty_port_trace_CORTEXM__write,
	.put_char =&tty_port_trace_CORTEXM__put_char,
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
int tty_porttrace_CORTEXM__share_if(void)
{
	tty_driver_register(&s_ttydriver_trace_CORTEXM);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_trace_CORTEXM__open(ttydevice_t *_ttydevice)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__open((trace_swo_hdl_t*)&_ttydevice->port_hdl,_ttydevice->ttydriver->info.deviceIndex);
	if (ret_val < EOK) {
		_ttydevice->port_hdl = NULL;
	}

	return ret_val;
}

static int tty_port_trace_CORTEXM__close(ttydevice_t *_ttydevice)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_trace_CORTEXM__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__vprintf((trace_swo_hdl_t)_ttydevice->port_hdl, _format, _ap);

	return ret_val;
}

static int tty_port_trace_CORTEXM__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_trace_swo__putchar((trace_swo_hdl_t)_ttydevice->port_hdl, _c);

	return ret_val;
}




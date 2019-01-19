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
#include <lib_console.h>

/* project */
#include <lib_console_factory.h>
#include <lib_ttyportmux_types.h>
#include "tty_portconsole.h"

/* *******************************************************************
 * defines
 * ******************************************************************/


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int tty_port_console__open(ttydevice_t *_ttydevice);
static int tty_port_console__close(ttydevice_t *_ttydevice);
static int tty_port_console__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap);
static int tty_port_console__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c);
static int tty_port_console__read (ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static ttydriver_t s_ttydriver_console = {
	.info.deviceName = "TTYDEVICE_console",
	.info.deviceType = TTYDEVICE_console,
	.info.deviceNumber = 1,
	.open = &tty_port_console__open,
	.close = &tty_port_console__close,
	.write = &tty_port_console__write,
	.put_char =&tty_port_console__put_char,
	.read = &tty_port_console__read,
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
int tty_portconsole__share_if(void)
{
	int deviceNumber;
	deviceNumber = lib_console_factory__instances();
	if (deviceNumber < EOK) {
		return deviceNumber;
	}
	s_ttydriver_console.info.deviceNumber = deviceNumber;

	tty_driver_register(&s_ttydriver_console);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_console__open(ttydevice_t *_ttydevice)
{
	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	_ttydevice->port_hdl = (void*)lib_console_factory__instance(_ttydevice->ttydriver->info.deviceIndex);
	if (_ttydevice->port_hdl == NULL) {
		return -EPAR_NULL;
	}
	return EOK;
}

static int tty_port_console__close(ttydevice_t *_ttydevice)
{
	int ret_val;
	return EOK;
}

static int tty_port_console__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__vprint_debug_message((console_hdl_t)_ttydevice->port_hdl, _format, _ap);
	return ret_val;
}

static int tty_port_console__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__putchar((console_hdl_t)_ttydevice->port_hdl,_c);
	return ret_val;
}

static int tty_port_console__read (ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	ret_val = lib_console__getdelim((console_hdl_t)_ttydevice->port_hdl, _lineptr, _n, _delimiter);
	return ret_val;
}



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
static int tty_port_unix__open(ttydevice_t *_ttydevice);
static int tty_port_unix__close(ttydevice_t *_ttydevice);
static int tty_port_unix__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap);
static int tty_port_unix__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c);
static int tty_port_unix__read (ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static ttydriver_t s_ttydriver_unix= {
	.info.deviceName = "TTYDEVICE_unix",
	.info.deviceType = TTYDEVICE_unix,
	.info.deviceNumber = 1,
	.open = &tty_port_unix__open,
	.close = &tty_port_unix__close,
	.write = &tty_port_unix__write,
	.put_char =&tty_port_unix__put_char,
	.read = &tty_port_unix__read,
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
int tty_portunix__share_if(void)
{
	tty_driver_register(&s_ttydriver_unix);
	return EOK;
}


/* *******************************************************************
 * static function definition
 * ******************************************************************/

static int tty_port_unix__open(ttydevice_t *_ttydevice)
{
	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}
	return EOK;
}

static int tty_port_unix__close(ttydevice_t *_ttydevice)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	return EOK;
}

static int tty_port_unix__write(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, const char * const _format, va_list _ap)
{
	int ret_val;

	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	vprintf(_format, _ap);
	fflush(stdout);
	return EOK;

}

static int tty_port_unix__put_char(ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char _c)
{
	if (_ttydevice == NULL) {
		return -ESTD_INVAL;
	}

	putchar(_c);
	fflush(stdout);
	return EOK;
}

static int tty_port_unix__read (ttydevice_t *_ttydevice, enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret_val;
	if (_ttydevice == NULL) {
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


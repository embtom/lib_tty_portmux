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

#ifndef INT_IO_TTY_LIB_TTYPORTMUX_SH_LIB_TTYPORTMUX_INC_INT_PORT_IF_PLUGIN_H_
#define INT_IO_TTY_LIB_TTYPORTMUX_SH_LIB_TTYPORTMUX_INC_INT_PORT_IF_PLUGIN_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c runtime */
#include <stdarg.h>
#include <stddef.h>
/* frame */
#include <lib_list_types.h>
/*project */
#include "lib_ttyportmux_types.h"

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
typedef struct ttydriver ttydriver_t;

struct ttydevice
{
	struct list_node node;
	void *port_hdl;
	ttydriver_t *ttydriver;		/*driver structure passed during init*/
};

typedef int (tty_open_t)(ttydevice_t *_ttydevice);
typedef int (tty_close_t)(ttydevice_t *_ttydevice);
typedef int (tty_write_t)(ttydevice_t *_ttydevice, enum ttyStreamType _stream, const char * const _format, va_list _ap);
typedef int (tty_put_char_t)(ttydevice_t *_ttydevice, enum ttyStreamType _stream, char _c);
typedef int (tty_read_t)(ttydevice_t *_ttydevice, enum ttyStreamType _stream, char *_lineptr, size_t *_n, char _delimiter);

/* ************************************************************************//**
 * \brief	actual structure which is used for exchanging the function
 * 			pointers.
 * ****************************************************************************/
struct ttydriver {
	struct ttyDeviceInfo info;
	tty_open_t *open;				/*!< initialization function */
	tty_close_t *close;			/*!< cleanup function */
	tty_write_t *write;				/*!<  */
	tty_put_char_t *put_char;		/*!< seek function */
	tty_read_t *read;
	ttydevice_t *ttydevice;
};




/* ************************************************************************//**
 * \brief	No Interface function only for internal -  Register a stdio channel at the port multiplexer
 *
 * \param   _ttydriver  share interfaces with the port multiplexer
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int tty_driver_register(ttydriver_t * const _port_if);

#endif /* INT_IO_TTY_LIB_TTYPORTMUX_SH_LIB_TTYPORTMUX_INC_INT_PORT_IF_PLUGIN_H_ */

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

#ifndef _LIB_TTYPORTMUX_TYPES_H_
#define _LIB_TTYPORTMUX_TYPES_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_STREAM_MAPPING_ENTRY(__port_type) \
{													  \
	.deviceType = __port_type,						  \
	.ttydevice = NULL								  \
}

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
enum ttyStreamType {
	TTYSTREAM_critical,
	TTYSTREAM_error,
	TTYSTREAM_warning,
	TTYSTREAM_info,
	TTYSTREAM_debug,
	TTYSTREAM_control,
	TTYSTREAM_CNT
};

enum ttyDeviceType {
	TTYDEVICE_none,
	TTYDEVICE_console,
	TTYDEVICE_trace_CORTEXM,
	TTYDEVICE_unix,
	TTYDEVICE_syslog
};


struct ttyStreamInfo {
	enum ttyStreamType streamType;
	char *streamName;
	enum ttyDeviceType deviceType;
	char *deviceName;
	unsigned int deviceIndex;
};

struct ttyDeviceInfo {
	enum ttyDeviceType deviceType;
	char *deviceName;
	unsigned int deviceIndex;
	unsigned int deviceNumber;
};


typedef struct ttydevice ttydevice_t;
struct ttyStreamMap
{
	enum ttyDeviceType deviceType;
 	enum ttyStreamType streamType;
	ttydevice_t *ttydevice;
};

#endif /* _LIB_TTYPORTMUX_TYPES_H_ */


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

#ifndef _LIB_TTYPORTMUX_H_
#define _LIB_TTYPORTMUX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * *******************************************************************/

/* c-runtime*/
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/*project*/
#include <lib_ttyportmux_types.h>

/* *******************************************************************
 * function declarations
 * ******************************************************************/

/* ************************************************************************//**
 * INIT CLEANUP
 * ****************************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the tty port multiplexer,
 *
 * \param	_deviceID_out : deviceID for out channel
 * \param 	_deviceID_in_out : deviceID for in out channel
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__init(struct ttyStreamMap *_map, size_t _mapSize);

/* ************************************************************************//**
 * \brief	Cleanup of the tty port multiplexer
 *
 * \param	_deviceID_out : deviceID for
 * \brief Printout a message on the serial console
 *
 * \param   _format 		"printf" style formatted string argument
 * \return	EOK if successful, or negative errno value on error
 *
 *  out channel
 * \param 	_deviceID_in_out : deviceID for in out channel
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__cleanup(void);

/* ************************************************************************//**
 * STDIO INTERFACE
 * ****************************************************************************/

/* ************************************************************************//**
 * \brief Printout a message on the tty port multiplexer
 *
 * \param   _streamType		Categorization of the in and output device
 * \param   _format 	"printf" style formatted string argument
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__print(enum ttyStreamType _streamType, const char * const _format, ...);

/* ************************************************************************//**
 *  \brief	Printout a variable argument list through tty port multiplexer
 *
 * \param   _streamType		Categorization of the requirements on the stdio device
 * \param   _format 	"printf" style formatted string argument
 * \param	_ap			variable argument list
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__vprint(enum ttyStreamType _streamType, const char * const _format, va_list _ap);

/* ************************************************************************//**
 *  \brief	Printout a character through the tty port multiplexer
 *
 * \param   _streamType	Categorization of the requirements at the stdio device
 * \param   _c 		Character to print
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_ttyportmux__putchar(enum ttyStreamType _streamType, char _c);

/* ************************************************************************//**
 *  \brief	Read of a full console log until the newline is reached
 *
 * \param   _streamType	Categorization of the requirements at the stdio device*
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_ttyportmux__getline(enum ttyStreamType _streamType, char *_lineptr, size_t *_n);

/* ************************************************************************//**
 *  \brief	 Read through tty port until the delimitaion character is found
 *
 * \param   _streamType	Categorization of the requirements at the stdio device
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \param	_delimiter		delimiter character to read line
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_ttyportmux__getdelim(enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter);

/* ************************************************************************//**
 * TTYDEVICE QUERY INTERFACE 
 * ****************************************************************************/

#define lib__ttyportmux_listentry_foreach(list_entry, first_entry)  \
        for(list_entry = first_entry; list_entry != NULL; list_entry = lib_ttyportmux__get_ttydevice_next(list_entry))

/* ************************************************************************//**
 * \brief	Request of the number of available tty out devices
 * \return	(ret>EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__ttydevice_count();

/* ************************************************************************//**
 * \brief	Get listentry of tty out devices
 * \return  pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct list_node* lib_ttyportmux__get_ttydevice_listentry();

/* ************************************************************************//**
 * \brief	Get next listentry of tty out devices
 * \param   struct list_node *_node		Current device
 * \return	pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct list_node* lib_ttyportmux__get_ttydevice_next(struct list_node *_node);

/* ************************************************************************//**
 * \brief	Get listentry of tty out devices
 * \return  pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct ttyDeviceInfo* lib_ttyportmux__get_ttydevice_info(struct list_node *_node);

/* ************************************************************************//**
 * TTYSTREAM QUERY INTERFACE 
 * ****************************************************************************/

/* ************************************************************************//**
 *  \brief	 Request of the number of provided streams
 *
 * \return	(ret > EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_count();

/* ************************************************************************//**
 *  \brief	 Request of the current ttystream to ttydevice mapping table
 *
 * \param   _map [out] : Base address of map to request
 * \param	_mapSize   : Size of the map to request
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_mapping(struct ttyStreamMap *const _map, size_t _mapSize);

/* ************************************************************************//**
 *  \brief	 Set of a new ttystream mapping table of ttydevices
 *
 * \param   _map [in]  : Base address of map to set
 * \param	_mapSize   : Size of the map to set
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__set_stream_mapping(const struct ttyStreamMap *const _map, size_t _mapSize);

/* ************************************************************************//**
 *  \brief	 Request the streamInfo to a corresponding streamMap
 *
 * \param   _map [in]		:	map entry to stream to request
 * \param	_streamInfo[OUT]:	return corresponding meta info of a streamInfo 
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_info(const struct ttyStreamMap *const _map, struct ttyStreamInfo * const _streamInfo);


#ifdef __cplusplus
}
#endif

#endif  /* LIB_TTYPORTMUX */

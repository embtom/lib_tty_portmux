/* ****************************************************************************************************
 * lib_port_mux.h for multiplexing of tty ports
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	15 August 2018	Tom				- creation of lib_tty_portmux.h
 *
 */
 
#ifndef _LIB_TTY_PORTMUX_H_
#define _LIB_TTY_PORTMUX_H_

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
#include "lib_tty_portmux_types.h"

/* *******************************************************************
 * forward declarations
 * ******************************************************************/
typedef struct list_node* device_list_t;

/* *******************************************************************
 * function declarations
 * ******************************************************************/

/* init cleanup */

/* ************************************************************************//**
 * \brief	Initialization of the tty port multiplexer,
 *
 * \param	_deviceID_out : deviceID for out channel
 * \param 	_deviceID_in_out : deviceID for in out channel
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__init(struct tty_stream_mapping * const _map, size_t _map_size);

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
int lib_tty_portmux__cleanup(void);

 /* stdio interface */

/* ************************************************************************//**
 * \brief Printout a message on the tty port multiplexer
 *
 * \param   _stream		Categorization of the in and output device
 * \param   _format 	"printf" style formatted string argument
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__print(enum tty_stream _stream, const char * const _format, ...);

/* ************************************************************************//**
 *  \brief	Printout a variable argument list through tty port multiplexer
 *
 * \param   _stream		Categorization of the requirements on the stdio device
 * \param   _format 	"printf" style formatted string argument
 * \param	_ap			variable argument list
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__vprint(enum tty_stream _stream, const char * const _format, va_list _ap);

/* ************************************************************************//**
 *  \brief	Printout a character through the tty port multiplexer
 *
 * \param   _stream	Categorization of the requirements at the stdio device
 * \param   _c 		Character to print
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_tty_portmux__putchar(enum tty_stream _stream, char _c);

/* ************************************************************************//**
 *  \brief	Read of a full console log until the newline is reached
 *
 * \param   _stream	Categorization of the requirements at the stdio device*
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_tty_portmux__getline(enum tty_stream _stream, char *_lineptr, size_t *_n);

/* ************************************************************************//**
 *  \brief	 Read through tty port until the delimitaion character is found
 *
 * \param   _stream	Categorization of the requirements at the stdio device
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \param	_delimiter		delimiter character to read line
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_tty_portmux__getdelim(enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter);

/* ************************************************************************//**
 * \brief	Request of the number of available stdio devices
 *
 * \return	(ret>EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_device_number();

/* ************************************************************************//**
 *  \brief	 Request of the device info of a available stdio device
 *
 * \param   _deviceID			DeviceID of the stdio to request
 * \param	_device_info[OUT]	Return by pointer of the stdio device info
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_device_info(device_list_t *_node, const struct device_info ** _info);

/* ************************************************************************//**
 *  \brief	 Request of the number of provided streams
 *
 * \return	(ret > EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_stream_number(void);

/* ************************************************************************//**
 *  \brief	 Request of the device info of a available stdio device
 *
 * \param   _deviceID			DeviceID of the stdio to request
 * \param	_device_info[OUT]	Return by pointer of the stdio device info
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_stream_info(enum tty_stream _stream, struct stream_info * const _device_info);

/* ************************************************************************//**
 *  \brief	 Set of the active out device ID
 *
 * \param   _deviceID			DeviceID of the stdio to set
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__set_stream_device(enum tty_stream _stream, const struct device_info *_info);


#ifdef __cplusplus
}
#endif

#endif  /* LIB_TTY_PORTMUX */

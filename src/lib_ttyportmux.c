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

/*c -runtime */
#include <string.h>
#include <stdarg.h>

/* frame */
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_list.h>
#include <lib_convention__mem.h>
#include <mini-printf.h>

/* project */
#include <tty_portplugin_init.h>
#include "tty_portplugin_if.h"
#include "lib_ttyportmux.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_LIB_LIST_CONTEXT_ID			0
#define M_LIB_LIST_BASE_ADDR			0

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static data
 * ******************************************************************/
static unsigned int s_initCount = 0;
static struct queue_attr s_ttydriverList;
static struct ttyStreamMap *s_streamMap = NULL;
static unsigned int s_streamMapCount = 0; 

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static ttydevice_t* lib_ttyportmux__stream_to_device(enum ttyStreamType _streamType);
static char* lib_ttyportmux__stream_name(enum ttyStreamType _streamType);

/* *******************************************************************
 * function definition
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the tty port multiplexer,
 *
 * \param	_deviceID_out : deviceID for out channel
 * \param 	_deviceID_in_out : deviceID for in out channel
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__init(struct ttyStreamMap *_map, size_t _mapSize)
{
	unsigned int map_count;
	struct list_node *ttydevice_node;
	ttydevice_t *ttydevice;
	tty_open_t *driver_open;
	int i, ret;

	map_count = _mapSize / sizeof(struct ttyStreamMap);

	if (_map == NULL) {
		return -EPAR_NULL;
	}

	if ((map_count < 0) || (map_count > TTYSTREAM_CNT)) {
		return -ESTD_INVAL;
	}

	if (s_initCount > 0) {
		s_initCount++;
	}

	s_streamMap = _map;
	s_streamMapCount = map_count;

	ret = lib_list__init(&s_ttydriverList, M_LIB_LIST_CONTEXT_ID);
	if (ret < EOK) {
		goto ERR_PLUGIN_LIST;
	}

	/*register of available plugins */
	tty_port_plugin();

	ret = lib_list__emty(&s_ttydriverList,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		ret = -ESTD_NODEV;
		goto ERR_BUS_LIST;
	}

	ret = lib_list__get_begin(&s_ttydriverList ,&ttydevice_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		goto ERR_BUS_LIST;
	}

	do {
		ttydevice = (ttydevice_t*)GET_CONTAINER_OF(ttydevice_node, struct ttydevice, node);

		if(ttydevice->ttydriver != NULL) {
			driver_open = ttydevice->ttydriver->open;
			if (driver_open != NULL) {
				ret = (*driver_open)(ttydevice);
			}

			if (ret != EOK) {
				lib_list__delete(&s_ttydriverList,&ttydevice->node,M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
			}
			else {

				for(i=0; i < s_streamMapCount; i++) {
					if(s_streamMap[i].deviceType == ttydevice->ttydriver->info.deviceType) {
						s_streamMap[i].ttydevice = ttydevice;
						s_streamMap[i].streamType = i;
					}
				}	
			}
		}
	}while(ret = lib_list__get_next(&s_ttydriverList,&ttydevice_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR), (ret == LIB_LIST__EOK));

	s_initCount = 1;

	return EOK;
	ERR_BUS_LIST:

	ERR_PLUGIN_LIST:
	return ret;

 }

/* ************************************************************************//**
 * \brief	Cleanup of the tty port multiplexer
 *
 * \param	_deviceID_out : deviceID for out channel
 * \param 	_deviceID_in_out : deviceID for in out channel
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
 int lib_ttyportmux__cleanup(void)
 {
 	 return EOK;
 }

 /* ************************************************************************//**
  * \brief Printout a message through tty port multiplexer
  *
  * \param   _streamType		Categorization of the requirements of the stdio device
  * \param   _format 	"printf" style formatted string argument
  *
  * \return	EOK if successful, or negative errno value on error
  * ****************************************************************************/
int lib_ttyportmux__print(enum ttyStreamType _streamType, const char * const _format, ...)
{
	int ret;
	va_list ap;
	ttydevice_t *ttydevice;

	if ((_streamType >= TTYSTREAM_CNT) || (_format == NULL)) {
		return -ESTD_INVAL;
	}

	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ttydevice = lib_ttyportmux__stream_to_device(_streamType);
	if (ttydevice == NULL) {
		return -ESTD_NODEV;
	}

	va_start(ap,_format);
	ret = (*ttydevice->ttydriver->write)(ttydevice, _streamType, _format,ap);
	va_end(ap);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Printout a variable argument list through tty port multiplexer
 *
 * \param   _streamType		Categorization of the requirements on the stdio device
 * \param   _format 	"printf" style formatted string argument
 * \param	_ap			variable argument list
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__vprint(enum ttyStreamType _streamType, const char * const _format, va_list _ap)
{
	int ret;
	ttydevice_t *ttydevice;

	if ((_streamType >= TTYSTREAM_CNT) || (_format == NULL)) {
		return -ESTD_INVAL;
	}

	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ttydevice = lib_ttyportmux__stream_to_device(_streamType);
	if (ttydevice == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*ttydevice->ttydriver->write)(ttydevice,_streamType,_format,_ap);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Printout a character through tty port multiplexer
 *
 * \param   _streamType	Categorization of the requirements at the stdio device
 * \param   _c 		Character to print
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_ttyportmux__putchar(enum ttyStreamType _streamType, char _c)
{
	int ret;
	ttydevice_t *ttydevice;

	if (_streamType >= TTYSTREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ttydevice = lib_ttyportmux__stream_to_device(_streamType);
	if (ttydevice == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*ttydevice->ttydriver->put_char)(ttydevice,_streamType,_c);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Read through tty port until the newline is reached
 *
 * \param   _streamType	Categorization of the requirements at the stdio device
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_ttyportmux__getline(enum ttyStreamType _streamType, char *_lineptr, size_t *_n)
{
	int ret;
	ttydevice_t *ttydevice;

	if (_streamType >= TTYSTREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ttydevice = lib_ttyportmux__stream_to_device(_streamType);
	if (ttydevice == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*ttydevice->ttydriver->read)(ttydevice,_streamType, _lineptr,_n,'\n');
	return EOK;
}

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
int lib_ttyportmux__getdelim(enum ttyStreamType _streamType, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret;
	ttydevice_t *ttydevice;

	if (_streamType >= TTYSTREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ttydevice = lib_ttyportmux__stream_to_device(_streamType);
	if (ttydevice == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*ttydevice->ttydriver->read)(ttydevice,_streamType, _lineptr,_n,_delimiter);
	return EOK;
}

/* ************************************************************************//**
 * \brief	Request of the number of available stdio devices
 *
 * \return	(ret>EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__ttydevice_count()
{
	int ret;
	if (s_initCount == 0) {
		return -EEXEC_NOINIT;
	}

	ret = lib_list__count(&s_ttydriverList,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		return -ESTD_NODEV;
	}
	return ret;
}

/* ************************************************************************//**
 * \brief	Get listentry of tty out devices
 * \return  pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct list_node* lib_ttyportmux__get_ttydevice_listentry()
{
	int ret;
	struct list_node *node;

	if (s_initCount == 0) {
		return NULL;
	}

	ret = lib_list__emty(&s_ttydriverList,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		return NULL;
	}

	ret = lib_list__get_begin(&s_ttydriverList , &node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		return NULL;
	}
	return node;
}

/* ************************************************************************//**
 * \brief	Get next listentry of tty out devices
 * \param   struct list_node *_node		Current device
 * \return	pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct list_node* lib_ttyportmux__get_ttydevice_next(struct list_node *_node)
{
	int ret;
	struct list_node *node = _node;
	ret = lib_list__get_next(&s_ttydriverList, &node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		return NULL;
	}
	return node;
}

/* ************************************************************************//**
 * \brief	Get listentry of tty out devices
 * \return  pointer to "list_node" if successful, or NULL on error
 * ****************************************************************************/
struct ttyDeviceInfo* lib_ttyportmux__get_ttydevice_info(struct list_node *_node) 
{
	struct ttydevice *ttydevice;
	struct ttyDeviceInfo *ttydevice_info;
	ttydevice = (struct ttydevice*)GET_CONTAINER_OF(_node, struct ttydevice, node);
	ttydevice_info = &ttydevice->ttydriver->info;
	return ttydevice_info;
}

/* ************************************************************************//**
 *  \brief	 Request of the number of provided streams
 *
 * \return	(ret > EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_count()
{
	return s_streamMapCount;
}

/* ************************************************************************//**
 *  \brief	 Request of the current ttystream to ttydevice mapping table
 *
 * \param   _map [out] : Base address of map to request
 * \param	_mapSize   : Size of the map to request
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_mapping(struct ttyStreamMap * const _map, size_t _mapSize)
{
	size_t streamMapSize = s_streamMapCount * sizeof(struct ttyStreamMap);

	if (_map == NULL) {
		return -EPAR_NULL;
	}

	if (streamMapSize > _mapSize) {
		return -ESTD_NOSPC;
	}

	memset(_map,0, _mapSize);
	memcpy(_map,s_streamMap, streamMapSize);
	return s_streamMapCount;
}

/* ************************************************************************//**
 *  \brief	 Set of a new ttystream mapping table of ttydevices
 *
 * \param   _map [in]  : Base address of map to set
 * \param	_mapSize   : Size of the map to set
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__set_stream_mapping(const struct ttyStreamMap * const _map, size_t _mapSize) 
{
	int ret;
	unsigned int i, entryCount;

	struct list_node *ttydevice_node;
	ttydevice_t *ttydevice;
	
	if (_map == NULL) {
		return -EPAR_NULL;
	}

	entryCount = _mapSize/sizeof(struct ttyStreamMap);
	if(entryCount > s_streamMapCount) {
		entryCount = s_streamMapCount;
	}

	ret = lib_list__get_begin(&s_ttydriverList ,&ttydevice_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		return ret;
	}

	for(i=0; i < s_streamMapCount; i++) {
		if(s_streamMap[i].deviceType != _map[i].deviceType) {
			s_streamMap[i].deviceType = _map[i].deviceType;
		}
	}

	do {
		ttydevice = (ttydevice_t*)GET_CONTAINER_OF(ttydevice_node, struct ttydevice, node);

		for(i=0; i < s_streamMapCount; i++) {
			if(s_streamMap[i].deviceType == ttydevice->ttydriver->info.deviceType) {
				s_streamMap[i].ttydevice = ttydevice;
			}
		}
	}while(ret = lib_list__get_next(&s_ttydriverList,&ttydevice_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR), (ret == LIB_LIST__EOK));
	return EOK;
}

/* ************************************************************************//**
 *  \brief	 Request the streamInfo to a corresponding streamMap
 *
 * \param   _map [in]		:	map entry to stream to request
 * \param	_streamInfo[OUT]:	return corresponding meta info of a streamInfo 
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_ttyportmux__get_stream_info(const struct ttyStreamMap *const _map, struct ttyStreamInfo * const _streamInfo)
{
	if ((_map == NULL) || (_streamInfo == NULL)) {
		return -EPAR_NULL;
	}

	_streamInfo->deviceIndex = _map->ttydevice->ttydriver->info.deviceIndex;
	_streamInfo->streamName = lib_ttyportmux__stream_name(_map->streamType);
	_streamInfo->streamType = _map->streamType;
	_streamInfo->deviceName = _map->ttydevice->ttydriver->info.deviceName;
	_streamInfo->deviceType = _map->ttydevice->ttydriver->info.deviceType;
}

/* ************************************************************************//**
 * \brief	No Interface function only for internal -  Register a stdio channel at the port multiplexer
 *
 * \param   _ttydriver  share interfaces with the port multiplexer
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int tty_driver_register(ttydriver_t * const _ttydriver)
{
	int i;
	ttydevice_t *ttydevice;
	unsigned int deviceNumber;

	 deviceNumber = _ttydriver->info.deviceNumber;

	 ttydevice = (ttydevice_t*)alloc_memory(deviceNumber, sizeof(ttydevice_t));
	 if(ttydevice == NULL) {
		 return -EPAR_NULL;
	 }

	 for(i = 0; i < deviceNumber; i++) {
		ttydevice[i].ttydriver = _ttydriver;
		ttydevice[i].ttydriver->info.deviceIndex = i;
		ttydevice[i].ttydriver->ttydevice = &ttydevice[i];
		lib_list__enqueue(&s_ttydriverList,&ttydevice[i].node,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	 }
	 return EOK;
}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/

/* ************************************************************************//**
 * \brief Request of the active stdio channel depending of the channel category
 *
 * \param   _streamType	Categorization of the requirements at the stdio device
 * \return	Pointer to stdio channel if successful, or NULL on error
 * ****************************************************************************/
static ttydevice_t* lib_ttyportmux__stream_to_device(enum ttyStreamType _streamType)
{
	ttydevice_t *ttydevice;

	if ((_streamType >= TTYSTREAM_CNT) || (s_streamMapCount <_streamType)) {
		return NULL;
	}

	ttydevice = s_streamMap[_streamType].ttydevice;
	return ttydevice;
 }

static char* lib_ttyportmux__stream_name(enum ttyStreamType _streamType)
{
	switch(_streamType) {
		case TTYSTREAM_critical		: return "TTYSTREAM_critical";
		case TTYSTREAM_error		: return "TTYSTREAM_error   ";
		case TTYSTREAM_warning		: return "TTYSTREAM_warning ";
		case TTYSTREAM_info			: return "TTYSTREAM_info    ";
		case TTYSTREAM_debug 		: return "TTYSTREAM_debug   ";
		case TTYSTREAM_control		: return "TTYSTREAM_control ";
		default:
		{
			static char name[20];
			memset(&name[0],0,sizeof(name));
			mini_snprintf(&name[0], sizeof(name), "TTY_STREAM_%u", (int)_streamType);
			return &name[0];
		}
	}
}
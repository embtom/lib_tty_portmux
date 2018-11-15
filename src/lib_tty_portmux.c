/* ****************************************************************************************************
 * lib_port_mux.c for multiplexing of tty ports
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
#include <tty_port_plugin_init.h>
#include "tty_port_plugin_if.h"
#include "lib_tty_portmux.h"

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
static unsigned int s_init_count = 0;
static struct queue_attr s_bus_list;
// static unsigned int s_available_in_out_count;
// static unsigned int s_available_out_count;
static unsigned int s_number_of_devices;
static struct tty_stream_mapping *s_stream_mapping = NULL;
static unsigned int s_stream_mapping_count = 0; 

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static tty_device_t * lib_tty_portmux__stream_to_device(enum tty_stream _stream);
static char* lib_tty_portmux__stream_name(enum tty_stream _stream);

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
int lib_tty_portmux__init(struct tty_stream_mapping * const _map, size_t _map_size)
{
	unsigned int map_count;
	struct list_node *tty_device_node;
	struct tty_device *port_device;
	tty_open_t *driver_open;
	int i, ret;

	map_count = _map_size / sizeof(struct tty_stream_mapping);

	if (_map == NULL) {
		return -EPAR_NULL;
	}

	if ((map_count < 0) || (map_count > TTY_STREAM_CNT)) {
		return -ESTD_INVAL;
	}

	if (s_init_count > 0) {
		s_init_count++;
	}

	s_stream_mapping = _map;
	s_stream_mapping_count = map_count;

	ret = lib_list__init(&s_bus_list, M_LIB_LIST_CONTEXT_ID);
	if (ret < EOK) {
		goto ERR_PLUGIN_LIST;
	}

	/*register of available plugins */
	tty_port_plugin();

	ret = lib_list__emty(&s_bus_list,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		ret = -ESTD_NODEV;
		goto ERR_BUS_LIST;
	}

	ret = lib_list__get_begin(&s_bus_list ,&tty_device_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	if (ret < EOK) {
		goto ERR_BUS_LIST;
	}

	do {
		port_device = (struct tty_device*)GET_CONTAINER_OF(tty_device_node, struct tty_device, node);

		if(port_device->port_driver != NULL) {
			driver_open = port_device->port_driver->open;
			if (driver_open != NULL) {
				ret = (*driver_open)(port_device);
			}

			if (ret != EOK) {
				lib_list__delete(&s_bus_list,&port_device->node,M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
			}
			else {

				for(i=0; i < s_stream_mapping_count; i++) {
					if(s_stream_mapping[i].device_type == port_device->port_driver->info.device_type) {
						s_stream_mapping[i].port_device = port_device;
					}
				}	
			}
		}
	}while(ret = lib_list__get_next(&s_bus_list,&tty_device_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR), (ret == LIB_LIST__EOK));

	s_init_count = 1;

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
 int lib_tty_portmux__cleanup(void)
 {
 	 return EOK;
 }

 /* ************************************************************************//**
  * \brief Printout a message through tty port multiplexer
  *
  * \param   _stream		Categorization of the requirements of the stdio device
  * \param   _format 	"printf" style formatted string argument
  *
  * \return	EOK if successful, or negative errno value on error
  * ****************************************************************************/
int lib_tty_portmux__print(enum tty_stream _stream, const char * const _format, ...)
{
	int ret;
	va_list ap;
	tty_device_t *tty_device;

	if ((_stream >= TTY_STREAM_CNT) || (_format == NULL)) {
		return -ESTD_INVAL;
	}

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	tty_device = lib_tty_portmux__stream_to_device(_stream);
	if (tty_device == NULL) {
		return -ESTD_NODEV;
	}

	va_start(ap,_format);
	ret = (*tty_device->port_driver->write)(tty_device, _stream, _format,ap);
	va_end(ap);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Printout a variable argument list through tty port multiplexer
 *
 * \param   _stream		Categorization of the requirements on the stdio device
 * \param   _format 	"printf" style formatted string argument
 * \param	_ap			variable argument list
 *
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__vprint(enum tty_stream _stream, const char * const _format, va_list _ap)
{
	int ret;
	tty_device_t *tty_device;

	if ((_stream >= TTY_STREAM_CNT) || (_format == NULL)) {
		return -ESTD_INVAL;
	}

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	tty_device = lib_tty_portmux__stream_to_device(_stream);
	if (tty_device == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*tty_device->port_driver->write)(tty_device,_stream,_format,_ap);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Printout a character through tty port multiplexer
 *
 * \param   _stream	Categorization of the requirements at the stdio device
 * \param   _c 		Character to print
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_tty_portmux__putchar(enum tty_stream _stream, char _c)
{
	int ret;
	tty_device_t *tty_device;

	if (_stream >= TTY_STREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	tty_device = lib_tty_portmux__stream_to_device(_stream);
	if (tty_device == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*tty_device->port_driver->put_char)(tty_device,_stream,_c);
	return ret;
}

/* ************************************************************************//**
 *  \brief	Read through tty port until the newline is reached
 *
 * \param   _stream	Categorization of the requirements at the stdio device
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_tty_portmux__getline(enum tty_stream _stream, char *_lineptr, size_t *_n)
{
	int ret;
	tty_device_t *tty_device;

	if (_stream >= TTY_STREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	tty_device = lib_tty_portmux__stream_to_device(_stream);
	if (tty_device == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*tty_device->port_driver->read)(tty_device,_stream, _lineptr,_n,'\r');
	return EOK;
}

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
int lib_tty_portmux__getdelim(enum tty_stream _stream, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret;
	tty_device_t *tty_device;

	if (_stream >= TTY_STREAM_CNT) {
		return -ESTD_INVAL;
	}

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	tty_device = lib_tty_portmux__stream_to_device(_stream);
	if (tty_device == NULL) {
		return -ESTD_NODEV;
	}

	ret = (*tty_device->port_driver->read)(tty_device,_stream, _lineptr,_n,_delimiter);
	return EOK;
}

/* ************************************************************************//**
 * \brief	Request of the number of available stdio devices
 *
 * \return	(ret>EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_device_number()
{
	int ret;
	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	ret = lib_list__emty(&s_bus_list,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		return -ESTD_NODEV;
	}

	return s_number_of_devices;
}

/* ************************************************************************//**
 *  \brief Iterate through the list of available stdio devices
 *
 * \param   _deviceID			DeviceID of the stdio to request
 * \param	_device_info[OUT]	Return by pointer of the stdio device info
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_device_info(device_list_t *_node, const struct device_info ** _info)
{
	struct tty_device *device;
	int ret;

	if (_info == NULL) {
		return -EPAR_NULL;
	}

	ret = lib_list__emty(&s_bus_list,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		return -ESTD_NODEV;
	}

	if (*_node == NULL) {
		ret = lib_list__get_begin(&s_bus_list ,_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	}
	else {
		ret = lib_list__get_next(&s_bus_list,_node, M_LIB_LIST_CONTEXT_ID, M_LIB_LIST_BASE_ADDR);
	}

	if (ret < EOK) {
		return -ESTD_NOENT;
	}

	device = (struct tty_device*)GET_CONTAINER_OF(*_node, struct tty_device, node);

	*_info = &device->port_driver->info;
	return EOK;
}


/* ************************************************************************//**
 *  \brief	 Request of the number of provided streams
 *
 * \return	(ret > EOK) if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_stream_number(void)
{
	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}
	return s_stream_mapping_count;
}

/* ************************************************************************//**
 *  \brief	 Request of the device info of a available stdio device
 *
 * \param   _deviceID			DeviceID of the stdio to request
 * \param	_device_info[OUT]	Return by pointer of the stdio device info
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__get_stream_info(enum tty_stream _stream, struct stream_info * const _device_info)
{
	int ret;
	unsigned int port_count = 0;
	struct list_node *tty_device_node;
	struct tty_device *tty_device;

	if (s_init_count == 0) {
		return -EEXEC_NOINIT;
	}

	if (_device_info == NULL) {
		return -EPAR_NULL;
	}

	ret = lib_list__emty(&s_bus_list,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	if (ret != EOK) {
		return -ESTD_NODEV;
	}

	if (_stream >= s_stream_mapping_count) {
		return -ESTD_NODEV;
	}

	_device_info->device_index = s_stream_mapping[_stream].port_device->port_driver->info.device_index;
	_device_info->stream_name = lib_tty_portmux__stream_name(_stream);
	_device_info->stream = _stream;
	_device_info->device_type_name = s_stream_mapping[_stream].port_device->port_driver->info.device_type_name;
	_device_info->device_type = s_stream_mapping[_stream].port_device->port_driver->info.device_type;

	return EOK;
}

/* ************************************************************************//**
 *  \brief	 Set of the active out device ID
 *
 * \param   _deviceID			DeviceID of the stdio to set
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int lib_tty_portmux__set_stream_device(enum tty_stream _stream, const struct device_info *_info)
{
	int ret;
	struct list_node *tty_device_node;
	tty_driver_t *tty_driver;
	tty_driver = (tty_driver_t *)GET_CONTAINER_OF(_info,struct tty_driver,info);

	s_stream_mapping[_stream].port_device = tty_driver->device;
	return EOK;
}

/* ************************************************************************//**
 * \brief	No Interface function only for internal -  Register a stdio channel at the port multiplexer
 *
 * \param   _port_driver  share interfaces with the port multiplexer
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int tty_port_register(tty_driver_t * const _port_driver)
{
	int i;
	tty_device_t *device;
	unsigned int device_number;

	 device_number = _port_driver->info.device_number;

	 device = (tty_device_t*)alloc_memory(device_number, sizeof(tty_device_t));
	 if(device == NULL) {
		 return -EPAR_NULL;
	 }

	 for(i = 0; i < device_number; i++) {
		device[i].port_driver = _port_driver;
		device[i].port_driver->info.device_index = i;
		device[i].port_driver->device = &device[i];
		s_number_of_devices++;
		lib_list__enqueue(&s_bus_list,&device[i].node,M_LIB_LIST_CONTEXT_ID,M_LIB_LIST_BASE_ADDR);
	 }
	 return EOK;
}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/

/* ************************************************************************//**
 * \brief Request of the active stdio channel depending of the channel category
 *
 * \param   _stream	Categorization of the requirements at the stdio device
 * \return	Pointer to stdio channel if successful, or NULL on error
 * ****************************************************************************/
static tty_device_t* lib_tty_portmux__stream_to_device(enum tty_stream _stream)
{
	tty_device_t *tty_device;

	if ((_stream >= TTY_STREAM_CNT) || (s_stream_mapping_count <_stream)) {
		return NULL;
	}

	tty_device = s_stream_mapping[_stream].port_device;
	return tty_device;
 }

static char* lib_tty_portmux__stream_name(enum tty_stream _stream)
{
	switch(_stream) {
		case TTY_STREAM_critical		: return "TTY_STREAM_critical";
		case TTY_STREAM_error			: return "TTY_STREAM_error   ";
		case TTY_STREAM_warning			: return "TTY_STREAM_warning ";
		case TTY_STREAM_info			: return "TTY_STREAM_info    ";
		case TTY_STREAM_debug 			: return "TTY_STREAM_debug   ";
		case TTY_STREAM_CONTROL			: return "TTY_STREAM_CONTROL ";
		default:
		{
			static char name[20];
			memset(&name[0],0,sizeof(name));
			mini_snprintf(&name[0], sizeof(name), "TTY_STREAM_%u", (int)_stream);
			return &name[0];
		}
	}
}
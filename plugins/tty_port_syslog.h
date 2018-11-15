/* ****************************************************************************************************
 * tty_port_unix.h within the following project: bld_device_cmake_Nucleo_STM32F401
 *
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	16.08.2018			thomas			- creation of tty_port_trace_CORTEXM.c
 *
 */

#ifndef _TTY_PORT_SYSLOG_H_
#define _TTY_PORT_SYSLOG_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* project */
#include <tty_port_plugin_if.h>

/* *******************************************************************
 * \brief	sharing the interfaces
 * ---------
 * \remark  The execution of a thread stops only at cancellation points
 * ---------
 * \param	_share	[in/out] :	pointer for sharing the interfaces
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int tty_port_syslog__share_if(void);

#endif /* _TTY_PORT_SYSLOG_H_ */

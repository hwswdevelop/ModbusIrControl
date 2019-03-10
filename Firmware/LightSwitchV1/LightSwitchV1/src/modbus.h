/* modbus.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __MODBUS_H_INCLUDED
#define __MODBUS_H_INCLUDED

#include "modbus_serial.h"

#define MODBUS_EXCEPTION_BIT															(0x80)
#define MODBUS_ILLEGAL_FUNCTION														(0x01)
#define MODBUS_ILLEGAL_DATA_ADDRESS												(0x02)
#define MODBUS_ILLEGAL_DATA_VALUE													(0x03)
#define MODBUS_SLAVE_DEVICE_FAILURE												(0x04)
#define MODBUS_ACKNOWLEDGE																(0x05)
#define MODBUS_SLAVE_DEVICE_BUSY													(0x06)
#define MODBUS_MEMORY_PARITY_ERROR												(0x08)
#define MODBUS_GATEWAY_PATH_UNAVAILABLE										(0x0A)
#define MODBUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND		(0x0B)


#endif //__MODBUS_H_INCLUDED
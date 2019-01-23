/* modbus_serial.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __MODBUS_SERIAL_H_INCLUDED
#define __MODBUS_SERIAL_H_INCLUDED

#include "stm8s.h"


typedef enum {
   RS485_NO_PARITY = 0,
   RS485_EVEN = 1,
   RS485_ODD  = 2
} RS485_PARITY, *pRS485_PARITY;

typedef enum _RS485_BAUD {
	RS485_BAUD_4800   = 0,
  RS485_BAUD_9600   = 1,
  RS485_BAUD_19200  = 2,
  RS485_BAUD_38400  = 3,
  RS485_BAUD_57600  = 4,
  RS485_BAUD_115200 = 5 
} RS485_BAUD, *pRS485_BAUD;

typedef enum _MODBUS_MODE {
  MODBUS_MODE_ASCII = 0,
  MODBUS_MODE_RTU   = 1
} MODBUS_MODE, *pMODBUS_MODE;

typedef struct _MODBUS_CONFIG {
	 uint8_t							id;
   MODBUS_MODE          mode;
   RS485_BAUD           baud;
   RS485_PARITY         parity;    
} MODBUS_CONFIG, *pMODBUS_CONFIG;


typedef enum {
  RS485_NO_ERROR           =  0,
  RS485_ERROR_WRONG_BAUD   = -1,
  RS485_ERROR_WRONG_PARITY = -2
} RS485_ERROR, *pRS485_ERROR;

typedef enum {
	MODBUS_FUTURE_PROCESSING_REQUIRED,
	MODBUS_NO_FUTURE_PROCESSING_REQUIRED
} MODBUS_PROC;

extern MODBUS_CONFIG gModbusConfig;
void modbusInit(void);
void loadSerialConfig(void);
void saveSerialConfig(void);
void saveDefaultSerialConfig(void);
RS485_ERROR rs485_init(pMODBUS_CONFIG pCfg);
void modbusTx(uint8_t *data, const uint8_t size);
extern MODBUS_PROC modbusRx(uint8_t *data, uint8_t size);

#endif // __MODBUS_SERIAL_H_INCLUDED
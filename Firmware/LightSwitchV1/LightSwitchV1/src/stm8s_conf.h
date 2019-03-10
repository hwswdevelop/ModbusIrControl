#ifndef __STM8S_CONF_H_INCLUDED
#define __STM8S_CONF_H_INCLUDED

#define MCK_FREQ                8000000
#define RS485_FRAME_SIZE        80


#if (MCK_FREQ == 4000000)
#define HSI_DIV_VAL      (0x02 << 3)
#define RS485_TIM4_PSC   (6)
#elif (MCK_FREQ == 8000000)
#define HSI_DIV_VAL      (0x01 << 3)
#define RS485_TIM4_PSC   (7)
#else
	#error "Please set appropriate external resonator frequency"
#endif

// EEPROM ADDRESS OF MODBUS PARAMS

#define EEPROM_ADDR_MODBUS_ID				(0x4000)
#define EEPROM_ADDR_MODBUS_MODE			(0x4001)
#define EEPROM_ADDR_MODBUS_BAUD			(0x4002)
#define EEPROM_ADDR_MODBUS_PARITY		(0x4003)
#define EEPROM_ADDR_MODBUS_STOP			(0x4004)
#define EEPROM_STORAGE_SIZE					(5)
#define EEPROM_ADDR_MODBUS_CRCL			(0x4005)
#define EEPROM_ADDR_MODBUS_CRCH		  (0x4006)

#define assert_param(X)

#endif //__STM8S_CONF_H_INCLUDED
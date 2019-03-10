/* eeprom.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "eeprom.h"

/*********************************************************
 * EEPROM write
 *********************************************************/
void eepromWrite(uint8_t* addr, uint8_t data){
		uint8_t i;
    // Don't write if values are equal
		for(i = 0; i < 10; i++){
			if (*addr == data) return; 
			// Unlock data memory
			FLASH->DUKR = 0xAE;
			FLASH->DUKR = 0x56;
			// Enable memory write
			*addr = ~data;
			*addr = data;     
			while (!(FLASH->IAPSR & ( (1 << 6) | (1 << 0))));   
			// Disable write access to option bytes
			// Lock data memory
			FLASH->IAPSR |= FLASH_IAPSR_DUL;
		}

}

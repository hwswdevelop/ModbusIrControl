/* eeprom.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __EEPROM_H_INCLUDED
#define __EEPROM_H_INCLUDED

#include "stm8s.h"
#include "crc.h"

void eepromWrite(uint8_t* addr, uint8_t data);

#endif //__EEPROM_H_INCLUDED
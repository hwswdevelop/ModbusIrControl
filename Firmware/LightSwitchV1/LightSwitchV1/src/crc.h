/* crc.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __CRC_H_INCLUDED
#define __CRC_H_INCLUDED

#include "stm8s.h"

uint16_t rtuCRC (const uint8_t *data, uint8_t length);

#endif //__CRC_H_INCLUDED
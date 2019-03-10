/* helper.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __HELPER_H_INCLUDED
#define __HELPER_H_INCLUDED

#include "stm8s.h"

void memset(void* mem, uint8_t val, uint16_t size);
void memcpy(void *dst, void *src, uint16_t size);

#define LE8(addr)					((uint8_t) *((uint8_t*)(addr)))
#define LE16(addr)				( LE8(addr) | ( ( (uint16_t)LE8(addr + 1) ) << 8) )


#define BE8(addr)					((uint8_t) *((uint8_t*)(addr)))
#define BE16(addr)				((((uint16_t)BE8(addr)) << 8) | (BE8((addr + 1))) )



#endif
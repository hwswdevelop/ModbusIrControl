/* helper.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "helper.h"

void memset(void* mem, uint8_t val, uint16_t size){
		uint8_t *ptr = (uint8_t*)mem; 
		while(size--) *ptr++ = 0;
}

void memcpy(void *dst, void *src, uint16_t size){
	 uint8_t *s = (uint8_t*) src;
	 uint8_t *d = (uint8_t*) dst;
	 while(size--) *d++ = *s++;
}
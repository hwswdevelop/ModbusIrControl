/* ds18b20.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __DS18B20_H_INCLUDED
#define __DS18B20_H_INCLUDED

#include "stm8s.h"

void ds18b20ReadTemp(void);
void ds18b20Init(void);
uint16_t ds18b20GetTemp(void);

#endif //__DS18B20_H_INCLUDED
/* clock.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __CLOCK_H_INCLUDED
#define __CLOCK_H_INCLUDED

#include "stm8s.h"

extern volatile uint8_t gExtClockEnabled;
void clock_init(void);


#endif // __CLOCK_H_INCLUDED
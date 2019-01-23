/* vtimer.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __VTIMER_H_INCLUDED
#define __VTIMER_H_INCLUDED

void vTimerInit(void);
void vTimerStart(uint8_t id, uint16_t time);
void vTimerStop(uint8_t id);
uint16_t vTimerGetTime(uint8_t id);

#endif //__VTIMER_H_INCLUDED
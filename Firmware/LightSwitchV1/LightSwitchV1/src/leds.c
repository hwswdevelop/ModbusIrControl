/* leds.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "leds.h"
#include "vtimer.h"

void ledsOnOff(uint8_t ledNo, LED_ON_OFF on){
	uint8_t ledMask = (1 << (6 - ledNo));
	switch(on){
		case LED_OFF:	   GPIOC->ODR &= ~ledMask; break;
		case LED_ON:  	 GPIOC->ODR |= ledMask;  break;
		case LED_INVERT: GPIOC->ODR ^= ledMask;  break;
	}
}

uint8_t getLedStatus(uint8_t ledNo){
	uint8_t ledMask = (1 << (6 - ledNo));
	uint8_t status = ( (GPIOC->ODR & ledMask) != 0 );
	return status;
}

void ledsInit(void){
}

void ledsUpdate(void){
}




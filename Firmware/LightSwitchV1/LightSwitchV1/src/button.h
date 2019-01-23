/* button.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __BUTTON_H_INCLUDED
#define __BUTTON_H_INCLUDED

#include "stm8s.h"

#define BUTTONS_COUNT 3

uint8_t getButtonState(uint8_t buttonNo);
uint16_t getButtonPushTimeMs(uint8_t buttonNo);
uint8_t getButtonPushed(uint8_t buttonNo);
uint8_t getButtonPulled(uint8_t buttonNo);
void clearButtonPushed(uint8_t buttonNo);
void clearButtonPulled(uint8_t buttonNo);
void clearButtonPushTime(uint8_t buttonNo);
void buttonsInit(void);

#endif // __BUTTON_H_INCLUDED
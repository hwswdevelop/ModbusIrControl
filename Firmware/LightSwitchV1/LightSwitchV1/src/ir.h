/* ir.h file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#ifndef __IR_H_INCLUDED
#define __IR_H_INCLUDED

#include "stm8s.h"

extern uint8_t gTim1Avalilable;

void irInit(void);
uint8_t getIrRxLen(void);
uint8_t clearIrRx(void);
uint8_t getIrRxByte(uint8_t ind);
uint16_t getIrRxPause(void);
uint16_t getIrRxPulse(void);

void irTxFrame(uint16_t pulseUs, uint16_t pauseUs, uint8_t *data, uint8_t bitCount);

#endif // __IR_H_INCLUDED
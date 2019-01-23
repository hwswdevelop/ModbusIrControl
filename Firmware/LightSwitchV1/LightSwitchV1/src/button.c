/* button.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "button.h"
#include "vtimer.h"

#define BUTTON_FILTER_TIME_IN_10MS  (4)
#define BUTTON_PUSH_TIME_MAX				((uint16_t)65535)

typedef struct {	
	uint8_t   filteredState;
	uint8_t   changed;
	uint8_t		state;
	uint8_t 	counter;
	uint16_t  pushTime;
} BUTTON_STATE, *pBUTTON_STATE;

uint8_t getButtonInputValue(uint8_t buttonNo);

static BUTTON_STATE gButtonsState[BUTTONS_COUNT];


void buttonsInit(void){
	uint8_t butNo = 0;
	for(butNo = 0; butNo < BUTTONS_COUNT; butNo++){
			pBUTTON_STATE bs = &gButtonsState[butNo];
			bs->state =  getButtonInputValue(butNo);
			bs->filteredState = bs->state;			
			bs->pushTime = bs->counter = bs->changed = 0;
	}
	vTimerStart(3, 50000);
}

uint8_t getButtonState(uint8_t buttonNo){
	uint8_t inputVal = 0;
  if (buttonNo < BUTTONS_COUNT) 
		inputVal = gButtonsState[buttonNo].filteredState;
	return inputVal;
}

uint8_t getButtonPushed(uint8_t buttonNo){
	uint8_t changed = 0;
  if (buttonNo < BUTTONS_COUNT) 
		changed = gButtonsState[buttonNo].changed & 0x01;
	return changed;
}

uint8_t getButtonPulled(uint8_t buttonNo){
	uint8_t changed = 0;
  if (buttonNo < BUTTONS_COUNT) 
		changed = (gButtonsState[buttonNo].changed >> 1) & 0x01;
	return changed;
}

uint16_t getButtonPushTimeMs(uint8_t buttonNo){
	uint16_t pushTime = 0;
	if (buttonNo < BUTTONS_COUNT){
		pushTime = gButtonsState[buttonNo].pushTime;
	}
	return pushTime;
}

void clearButtonPushed(uint8_t buttonNo){
  if (buttonNo < BUTTONS_COUNT){ 
		gButtonsState[buttonNo].changed  &= ~(0x01);
	}
}

void clearButtonPulled(uint8_t buttonNo){
  if (buttonNo < BUTTONS_COUNT){ 
		gButtonsState[buttonNo].changed  &= ~(0x02);
	}
}


void clearButtonPushTime(uint8_t buttonNo){
  if (buttonNo < BUTTONS_COUNT){ 
		gButtonsState[buttonNo].pushTime = 0x00;
	}
}



uint8_t getButtonInputValue(uint8_t buttonNo){
	uint8_t inputVal = 0;
	switch(buttonNo){
			case 0:	inputVal = ( ~GPIOB->IDR >> 5 ) & 0x01; break;
			case 1: inputVal = ( ~GPIOB->IDR >> 4 ) & 0x01; break;
			case 2: inputVal = ( ~GPIOC->IDR >> 3 ) & 0x01; break;
	}
	return inputVal;
}

void vTimer3Irq(void){
	uint8_t butNo = 0;
	for(butNo = 0; butNo < BUTTONS_COUNT; butNo++){
			pBUTTON_STATE bs = &gButtonsState[butNo];
			uint8_t curState = getButtonInputValue(butNo);
			if (bs->state != curState){
					if (bs->counter < BUTTON_FILTER_TIME_IN_10MS){
						// Wait some cycles
						bs->counter++;
					} else {
						// will be here only once when state changed
						bs->filteredState = curState;						
						bs->changed |= (curState) ? 0x01 : 0x02;
						bs->state = curState;						
					}
			} else {
				bs->counter = 0;
				if ( (bs->changed == 0x01) &&
						 (bs->pushTime < BUTTON_PUSH_TIME_MAX) ){
						if (!bs->pushTime)
							bs->pushTime =	(((uint16_t)(10)) * BUTTON_FILTER_TIME_IN_10MS);
						else {
							if (bs->pushTime > ( (uint16_t)65520) )
								bs->pushTime = 65535;
							else
								bs->pushTime += 10;
						}
				}
			}
	}
	vTimerStart(3, 10000);
}


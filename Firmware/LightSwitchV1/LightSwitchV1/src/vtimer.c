/* vtimer.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */

#include "stm8s_conf.h"
#include "stm8s.h"
#include "helper.h"
#include "vtimer.h"


#if (MCK_FREQ == 8000000)
#define TIMER_TIM1_PSC   (7)
//#define TIMER_TIM1_PSC   (7999)
#elif (MCK_FREQ == 4000000)
#define TIMER_TIM1_PSC   (3)
//#define TIMER_TIM1_PSC   (3999)
#else
	#error "Please set appropriate external resonator frequency"
#endif

#define TIMER_TIM1_PSCRH   (TIMER_TIM1_PSC >> 8)
#define TIMER_TIM1_PSCRL   (TIMER_TIM1_PSC & 0xFF)

static uint16_t gRefTime[4];

void vTimer1Irq(void);
void vTimer2Irq(void);
void vTimer3Irq(void);
void vTimer4Irq(void);

INTERRUPT_HANDLER(TIM1_OvfIntVector, 13){
	TIM1->SR1 &= ~(TIM1_SR1_CC4IF | TIM1_SR1_CC3IF | 
								 TIM1_SR1_CC2IF | TIM1_SR1_CC1IF | 
								 TIM1_SR1_UIF);
	TIM1->CR1 |= TIM1_CR1_UDIS;
	TIM1->IER &= ~(TIM1_IER_UIE);
}

INTERRUPT_HANDLER(TIM1_CompIntVector, 14){
	uint8_t status = (TIM1->SR1 & TIM1->IER);
	uint8_t chan;	
	for(chan = 1; chan <= 4; chan++){
		uint8_t mask = 1 << chan;
		if (status & mask){
			uint8_t nmask = ~mask;
			TIM1->SR1 &= nmask;
			TIM1->IER &= nmask;
			// vTimer interrupt call from here...
			switch(chan){
				case 1: vTimer1Irq(); break;
				case 2: vTimer2Irq(); break;
				case 3: vTimer3Irq(); break;
				case 4: vTimer4Irq(); break;
			}			
		}
	}		
}

void vTimerInit(void){
	TIM1->CR1 =  TIM1_CR1_CEN;
	TIM1->ARRH = 0xFF;
	TIM1->ARRL = 0xFF;
	TIM1->PSCRH = TIMER_TIM1_PSCRH;
	TIM1->PSCRL = TIMER_TIM1_PSCRL;
	TIM1->IER = TIM1_IER_UIE;
	TIM1->EGR = TIM1_EGR_UG;
}

void vTimerStart(uint8_t id, uint16_t time){
	uint16_t curTime = ((uint16_t)(TIM1->CNTRH)) << 8;
	curTime |= TIM1->CNTRL;	
	{
		uint16_t endTime = curTime + time;
		uint8_t  valid = 1;
		switch(id){
			case 1: 
				TIM1->CCR1H = (uint8_t)(endTime >> 8); 
				TIM1->CCR1L = (uint8_t)endTime;
				break;
			case 2: 
				TIM1->CCR2H = (uint8_t)(endTime >> 8);
				TIM1->CCR2L = (uint8_t)endTime;
				break;
			case 3: 
				TIM1->CCR3H = (uint8_t)(endTime >> 8);
				TIM1->CCR3L = (uint8_t)endTime;
				break;
			case 4: 
				TIM1->CCR4H = (uint8_t)(endTime >> 8);
				TIM1->CCR4L = (uint8_t)endTime;
				break;
			default: valid = 0;
		}
		if (valid){
			// Save timer start time
			gRefTime[id - 1] = curTime;
			// Clear flag
			TIM1->SR1 &= ~(1 < id);
			// Enable interrupt
			TIM1->IER |= (1 << id);
		}
	}	
}

void vTimerStop(uint8_t id){
	if ( (id >= 1) && (id <= 4) ) {
			uint8_t nmask = ~(1 << id);
			// Disable interrupt
			TIM1->IER &= nmask;
			// Clear flag
			TIM1->SR1 &= nmask;
	}
}

uint16_t vTimerGetTime(uint8_t id){
	uint16_t curTime = ((uint16_t)(TIM1->CNTRH)) << 8;
	curTime |= TIM1->CNTRL;
	if ( (id >= 1) && (id <= 4) ) {
		return (curTime - gRefTime[id - 1]);
	} else return 0;
}


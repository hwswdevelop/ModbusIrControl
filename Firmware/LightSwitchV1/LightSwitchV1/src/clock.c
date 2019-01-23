/* clock.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "clock.h"
#include "stm8s_conf.h"


#define CLK_SWR_HSE_VAL			(0xB4)
#define CLK_SWR_HSI_VAL			(0xE1)

volatile uint8_t  gExtClockEnabled = 0;

/*********************************************************
 * Init Clock. 
 * Enable HSE if possible or HSI otherwise            
 *********************************************************/
void clock_init(void){
	uint16_t timeout = 30000; // Wait timeout for clock switching  
  // Enable clock divider
  CLK->CKDIVR = HSI_DIV_VAL;
  // Allow clock switching
	CLK->SWCR |= CLK_SWCR_SWEN;	
  // Switch to HSE
  CLK->SWR = CLK_SWR_HSE_VAL;
  // Wait for HSE ready or timeout
  while( (CLK->CMSR != CLK_SWR_HSE_VAL) && --timeout);
  if (!timeout) {
      // HSE not ready, swithing to HSI    
      CLK->SWR = CLK_SWR_HSI_VAL;
      while(CLK->CMSR != CLK_SWR_HSI_VAL);
			CLK->ECKR &= ~CLK_ECKR_HSEEN;
  } else {
    // HSE ready, enabling additional protection
    gExtClockEnabled = 1;
    CLK->CSSR |= CLK_CSSR_CSSDIE | CLK_CSSR_CSSEN; // Enabling CSS and it's interrupt
  }
  // Disable clock switching
  CLK->SWCR &= ~CLK_SWCR_SWEN;
}


/*********************************************************
 * Clock interrupt
 * Switch to internal 4Mhz/8Mhz if external clock is bad 
 *********************************************************/
 INTERRUPT_HANDLER(CLK_IRQHandler, 4){
  if (CLK->CSSR & CLK_CSSR_CSSD){
		// Clock security interrupt. 
    CLK->CSSR &= ~CLK_CSSR_CSSD;
		// Switching to internal clock. Set divider		
    CLK->CKDIVR = HSI_DIV_VAL;
    gExtClockEnabled = 0;
  }
}


/* main.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru, hwswdevelop@gmail.com
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
 
#include "stm8s_conf.h"
#include "stm8s.h"
#include "clock.h"
#include "modbus.h"
#include "modbus_serial.h"
#include "ir.h"
#include "leds.h"
#include "eeprom.h"
#include "helper.h"
#include "button.h"
#include "ds18b20.h"
#include "vtimer.h"

void gpio_init(void);
void irq_prio_init(void);
uint8_t check_reset(void);

void main()
{
		uint16_t counter = 0;
		clock_init();
		if ( check_reset() ){
			saveDefaultSerialConfig();
		}
		gpio_init();
		vTimerInit();
		buttonsInit();
		ledsInit();
		modbusInit();
		irInit();
		ds18b20Init();
		irq_prio_init();
		enableInterrupts();
		while(1){
      if (!counter++) ds18b20ReadTemp();
		}
}


/*********************************************************
 * IRQ Vector priority setup
 *********************************************************/
void irq_prio_init(void){
	ITC->ISPR1 = 0xC3;
	ITC->ISPR2 = 0x00;
	ITC->ISPR3 = 0x00;
	ITC->ISPR4 = 0x00;
	ITC->ISPR5 = 0x30;
	ITC->ISPR6 = 0x00;
	ITC->ISPR7 = 0x00;
}

/*********************************************************
 * Reset to default settings
 *********************************************************/
uint8_t check_reset(void){
	uint8_t count, wait;
	// Reset IR LED
	GPIOD->ODR &= ~(1 << 3);
  GPIOD->DDR |= (1 << 3);
	GPIOD->CR1 |= (1 << 3);
	GPIOD->CR2 |= (1 << 3);
	// Config test pins
	GPIOC->DDR = ( 1 << 5 );
  GPIOC->CR1 = ( 0x07 << 4 );
  GPIOC->CR2 = ( 0 << 4 );
	for(count = 0; count < 16; count++){
		GPIOC->ODR = (1 << 5);
		for(wait = 0; wait < 16; wait++);
		if ( ( (GPIOC->IDR >> 4) & 0x05 ) != 0x05 ) return 0;
		GPIOC->ODR = (0 << 5);
		for(wait = 0; wait < 16; wait++);
		if ( ( (GPIOC->IDR >> 4) & 0x05 ) != 0x00 ) return 0;		
	}
	return 1;
}


/*********************************************************
 * GPIO and Peripherals PINS
 *********************************************************/
void gpio_init(void){
  // PA3
  GPIOA->DDR =  0;
  GPIOA->CR1 =  (1 << 3);
  GPIOA->CR2 =  (1 << 3);
  
  // PB4 & PB5 as input
  GPIOB->DDR =  0;
  GPIOB->CR1 =  (0x03 << 4);
  GPIOB->CR2 =  (0x00 << 4);

  /*****************************/
  //  Port C - Real configuration
  //              PC3       PC4,5,6       PC7
  GPIOC->DDR = (0 << 3) | (0x07 << 4) | (0 << 7);
  GPIOC->CR1 = (1 << 3) | (0x07 << 4) | (1 << 7);
  GPIOC->CR2 = (0 << 3) | (0x00 << 4) | (0 << 7);
  GPIOC->ODR = (0 << 3) | (0 << 7); 
  
  // Configure RX, TX, AND pin used for TX Enable
  // RX    - PD6 (Pullup without interrupt)
  // TX    - PD5 (Output)
  // TX_EN - PD4 (Output)
  GPIOD->DDR =   (1 << 3) | (1 << 4) | (1 << 5) | (0 << 6);
  GPIOD->CR1 =   (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
  GPIOD->CR2 =   (1 << 3) | (1 << 4) | (1 << 5) | (0 << 6);
  GPIOD->ODR =    0; 
  
  // Confgure PortD interrupt by falling edge only
  //            PD         PC         PB         PA
  //EXTI->CR1 = (2 << 6) | (3 << 4) | (3 << 2) | (2 << 0);
  EXTI->CR1 = (2 << 0);
}





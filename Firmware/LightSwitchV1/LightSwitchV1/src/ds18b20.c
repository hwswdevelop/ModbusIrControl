/* ds18b20.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "ds18b20.h"
#include "vtimer.h"


#if (MCK_FREQ == 8000000)

#define TIMER_TIM1_PSC   (7)
#define DS_WAIT_CYCLES   (5)

#elif (MCK_FREQ == 4000000)

#define TIMER_TIM1_PSC   (3)
#define DS_WAIT_CYCLES   (2)

#else
	#error "Please set appropriate external resonator frequency"
#endif


#define TIMER_TIM1_PSCRH   (TIMER_TIM1_PSC >> 8)
#define TIMER_TIM1_PSCRL   (TIMER_TIM1_PSC & 0xFF)

#define OW_GPIO						GPIOC
#define OW_GPIO_PIN				( 7 )
#define OW_GPIO_MASK			( 1 << OW_GPIO_PIN )
#define OW_SET_LOW()			OW_GPIO->DDR |= OW_GPIO_MASK
#define OW_SET_HIGH()			OW_GPIO->DDR &= ~(OW_GPIO_MASK)

#define OW_HARD_PULLUP()	{ OW_GPIO->ODR |= OW_GPIO_MASK; \
														OW_GPIO->DDR |= OW_GPIO_MASK; }

#define OW_SOFT_PULLUP()	{	OW_GPIO->DDR &= ~OW_GPIO_MASK; \
														OW_GPIO->ODR &= ~OW_GPIO_MASK; }
				

#define DS18B20_CMD_SKIP_ROM		(0xCC)


typedef struct {
	float					tempC;
	uint16_t			temp;
} DS18B20_STATE, *pDS18B20_STATE;


DS18B20_STATE gDS18B20ST;

void ds18b20Wait5us(void);
void ds18b20WaitUs(uint16_t time);

volatile uint8_t gDS18B20TimerReady = 0;

void ds18b20WaitUs(uint16_t time);

void ds18b20Init(void){
	gDS18B20ST.temp = 0x5555;
	gDS18B20ST.tempC = 22.4567; gDS18B20ST.temp * 0.0625;
}

void ds18b20PulseLow(void){
	OW_SET_LOW();
	ds18b20Wait5us();
	OW_SET_HIGH();
}

uint8_t ds18b20PulseReset(void){
	uint8_t res = 0;
	OW_SET_HIGH();
	ds18b20WaitUs(5000);
	res = ((OW_GPIO->IDR & OW_GPIO_MASK) > 0);
	OW_SET_LOW();
	ds18b20WaitUs(470);
	OW_SET_HIGH();
	ds18b20WaitUs(60);
	res &= ((OW_GPIO->IDR & OW_GPIO_MASK) == 0);
	ds18b20WaitUs(235);	
}

void ds18b20Wait5us(void){
  uint8_t cntr;
	for(cntr = 0; cntr < DS_WAIT_CYCLES; cntr++)	_asm(" nop");
}

uint8_t ds18b20ReadWrite(uint8_t byte){
	uint8_t data = byte;
	uint8_t i = 0, inp = 0x00;
	for(i = 0; i <= 7; i++){
		OW_SET_LOW();
		if (data & 0x01){
			ds18b20Wait5us();
			OW_SET_HIGH();
			ds18b20Wait5us();
			inp = (OW_GPIO->IDR << (7 - OW_GPIO_PIN)) & 0x80;
			ds18b20WaitUs(40);
		} else {
			ds18b20WaitUs(50);
			OW_SET_HIGH();
		}
		data = ((data >> 1) & 0x7F) | inp;
		while(!(OW_GPIO->IDR & OW_GPIO_MASK));
		ds18b20Wait5us();
	}
	return data;
}


void ds18b20Write(uint8_t byte){
	uint8_t data = byte;
	uint8_t i = 0;
	for(i = 0; i <= 7; i++){
		OW_SET_LOW();
		if (data & 0x01){
			ds18b20Wait5us();
			OW_SET_HIGH();
			if (i < 7) ds18b20WaitUs(45);
		} else {
			ds18b20WaitUs(50);
			OW_SET_HIGH();
		}
		data = ((data >> 1) & 0x7F);
		while(!(OW_GPIO->IDR & OW_GPIO_MASK));
		if (i < 7) ds18b20Wait5us();
	}
}


void vTimer2Irq(void){
	gDS18B20TimerReady = 1;
}

void ds18b20WaitUs(uint16_t time){
	gDS18B20TimerReady = 0;
	vTimerStart(2, time);
	while(!gDS18B20TimerReady);
}


uint8_t ds18b20Crc(uint8_t data, uint8_t seed){
		uint8_t bitsLeft, temp;		
    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ data) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        data >>= 1;
    }
    return seed;
}


uint8_t ds18b20getTemp(uint16_t *temp){
	uint8_t  seed = 0x00;
	uint8_t scratch[9];
	uint8_t i, val;
	// Check for high level
	if (!ds18b20PulseReset()) return;
	// Start convert command 
	ds18b20ReadWrite(0xCC);
	ds18b20Write(0x44);
	// Wait for conversion stopped
	OW_HARD_PULLUP();
	for(i = 0 ; i < 21; i++) ds18b20WaitUs(50000);
	OW_SOFT_PULLUP();
	// Read temperature
	if (!ds18b20PulseReset()) return;
	ds18b20ReadWrite(0xCC);
	ds18b20ReadWrite(0xBE);
	// Reading scratchpad
	for(i = 0; i < 9; i++){
		val = ds18b20ReadWrite(0xFF);
		seed = ds18b20Crc(val, seed);
		scratch[i] = val;
	}
	
	if (!seed){		
		// Successfully read
		uint16_t t = scratch[0];
		t |= ((uint16_t)scratch[1]) << 8;
		if (temp) *temp = t;
		return 1;
	} else 
		return 0;
	
}

void ds18b20ReadTemp(void){
	uint16_t temp;
	int16_t  tempSign;
	float    tempC;
	// Now I can't disable interrupts, because of other tasks
	// Try to read temperature, timings can be wrong....
	if ( ds18b20getTemp(&temp) ){
		if (temp == 1360){
			// Check for 85C
			if (ds18b20getTemp(&temp)){
				if (temp == 1360){
					// Check for temp again
					if ( ds18b20getTemp(&temp)){
						if (temp == 1360){
							//Ñheck for temp again
							if (!ds18b20getTemp(&temp)) return;
							// If we are here it is really T = 85C
						}
					} else return;
				}
			} else return;
		}
	} else return;
  // Update temperature value
	
	tempSign = (int16_t)temp;
	tempC = tempSign * 0.0625;
	
	disableInterrupts();
	gDS18B20ST.temp = temp;
	gDS18B20ST.tempC = tempC;
	enableInterrupts();
}

uint16_t ds18b20GetTempC(uint8_t high){
	uint16_t result;
	if (high){
		result = ((uint16_t*)(&gDS18B20ST.tempC))[1];
	} else {
		result = ((uint16_t*)(&gDS18B20ST.tempC))[0];
	}	
	return result;
}

uint16_t ds18b20GetTemp(void){
	return gDS18B20ST.temp;
}
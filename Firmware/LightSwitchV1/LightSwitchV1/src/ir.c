/* ir.c file
 * 
 * Copyright (c) 2016-2018 Evgeny Sobolev
 * Contact:
 *	e-mail: evgeny@vrnnet.ru
 *  skype:  evgenysbl
 *  tel.:   +7(908)1375847
 */
#include "ir.h"
#include "stm8s.h"
#include "helper.h"
#include "ds18b20.h"
#include "vtimer.h"

#if (MCK_FREQ == 8000000)
#define TIM2_PSC_VAL			(2)
#elif (MCK_FREQ == 4000000)
#define TIM2_PSC_VAL			(1)
#endif


#define IR_DATA_PULSE_TIME			(560)
#define IR_DATA_PAUSE_1_TIME		(1690)
#define IR_DATA_PAUSE_0_TIME		(560)
#define IR_WAIT_AFTER_TX				(10000)

#define IR_WAIT_AFTER_TX_H			(IR_WAIT_AFTER_TX >> 8)
#define IR_WAIT_AFTER_TX_L			(IR_WAIT_AFTER_TX & 0xFF)


/*********************************************************
 * IR Receiver / Transmitter
 *********************************************************/
typedef enum {
	IR_MODE_RECEIVE = 0,
	IR_MODE_TRANSMIT
} IR_MODE;
 
 
typedef enum {
	IR_RX_STATE_IDLE = 0,
	IR_RX_STATE_PULSE,
	IR_RX_STATE_PAUSE,
	IR_RX_STATE_RECV,
	IR_RX_STATE_RECV_END,
	IR_RX_STATE_EMPTY
} IR_RX_STATE;
 
typedef enum {
	IR_TX_STATE_IDLE = 0,
	IR_TX_STATE_PULSE,
	IR_TX_STATE_PAUSE,
	IR_TX_STATE_DATA_PULSE,
	IR_TX_STATE_DATA_PAUSE,
	IR_TX_STATE_DATA_END,
	IR_TX_STATE_EMPTY
} IR_TX_STATE;
  
typedef struct {
	uint16_t 			ir_pulse_time;
	uint16_t 			ir_pause_time;
  IR_TX_STATE		state;	
	uint8_t				tx_size;
	uint8_t 			ir_frame[12];	
	uint8_t				tx_ind;
} IR_TX_ST;

typedef struct {
	uint16_t			ir_pulse_time;
	uint16_t			ir_pause_time;
	IR_RX_STATE		state;
	uint8_t				ir_sz;
	uint8_t				ir_rdy_frame[12];
	uint8_t				ir_frame[12];
	uint8_t				ir_ind;
} IR_RX_ST;

typedef struct {
	IR_MODE 	mode;	
	union {
		IR_TX_ST 	tx;
		IR_RX_ST 	rx;
	};
} IR_STATE, *pIR_STATE;

static IR_STATE gIRState;


void startIrTimerRx(void);
void swithIrRxToIdle(void);



uint8_t getIrRxLen(void){
	return gIRState.rx.ir_sz;
}

uint8_t getIrRxByte(uint8_t ind){
	return gIRState.rx.ir_rdy_frame[ind];
}

uint8_t clearIrRx(void){
	gIRState.rx.ir_sz = 0;
}

uint16_t getIrRxPause(void){
	return gIRState.rx.ir_pause_time;
}

uint16_t getIrRxPulse(void){
	return gIRState.rx.ir_pulse_time;
}


void irInit(void){
	  memset( &gIRState, 0, sizeof(gIRState));
		swithIrRxToIdle();
}
 
void startIrTimerRx(void){
		vTimerStart(1, 20000);
}

void swithIrRxToIdle(void){
		gIRState.rx.state = IR_RX_STATE_IDLE;
		vTimerStop(1);
		// Enable GPIOA interrupt
		GPIOA->CR2 |= (1 << 3);		
		// Config GPIOA falling edge interrupt
		EXTI->CR1 &= ~EXTI_CR1_PAIS;
		EXTI->CR1 |= 0x02;
}

// IR sensor interrupt
INTERRUPT_HANDLER(GPIOA_IntVector, 5){
	pIR_STATE st = &gIRState;
		
	if ( st->mode == IR_MODE_RECEIVE ){
		
			uint16_t time;
			// Stop timer    
			vTimerStop(1);
			// Get current time
			time = vTimerGetTime(1);
			// Check mode
			switch(st->rx.state){
				case IR_RX_STATE_IDLE:
				{
					if (st->rx.ir_sz) break;
					// First edge
					startIrTimerRx();
					st->rx.ir_ind = 0;
					st->rx.state = IR_RX_STATE_PULSE;
					EXTI->CR1 &= ~EXTI_CR1_PAIS;
					EXTI->CR1 |= 0x01;
				}
				break;
				
				case IR_RX_STATE_PULSE:
				{
					if ( (time > 3500) && (time < 15000) ){
							startIrTimerRx();
							st->rx.ir_pulse_time = time;
							st->rx.state = IR_RX_STATE_PAUSE;
							EXTI->CR1 &= ~EXTI_CR1_PAIS;
							EXTI->CR1 |= 0x02;
					} else {
							swithIrRxToIdle();
					}
				}
				break;
				
				case IR_RX_STATE_PAUSE:
				{					
					if ( (time > 3500) && (time < 10000) ){
							startIrTimerRx();
							st->rx.ir_pause_time = time;
							st->rx.state = IR_RX_STATE_RECV;
					} else {
							swithIrRxToIdle();
					}
				}	
				break;
				
				case IR_RX_STATE_RECV:
				{
					startIrTimerRx();
					if ( (0 < time ) && ( time < 3500) ){
							// Yes this is bit time
							uint8_t byte_ind = st->rx.ir_ind >> 3;
							uint8_t bit_ind = st->rx.ir_ind & 0x07;
							// Try to save received bit
							if (byte_ind < sizeof(st->rx.ir_frame)){
								// NEC IR remote bit time is 1.12 ms of "0" and 2.25 ms of "1"
								if (time > 1685) 
									st->rx.ir_frame[byte_ind] |= (1 << bit_ind);
								else
									st->rx.ir_frame[byte_ind] &= ~(1 << bit_ind);
								if (st->rx.ir_ind < ( sizeof(st->rx.ir_frame) * 8 ) ) st->rx.ir_ind++;
							}
					} else {
						 st->rx.state = IR_RX_STATE_RECV_END;
					}
				}
				break;
				
				case IR_RX_STATE_RECV_END:
				{
					startIrTimerRx();
				}
				break;
			}
	}
}


// Configure timer2 to 38KHz
void configTimer2Tx(void){
		// Setup precaler to 1 MHz	
	TIM2->PSCR = TIM2_PSC_VAL;
	// Setup preload to 38KHz	
	TIM2->ARRH = 0x00;
	TIM2->ARRL = 0x34;
	// Enable auto preload
	TIM2->CR1 |= TIM2_CR1_ARPE;	
	TIM2->EGR |= TIM2_EGR_UG;
	TIM2->SR1 &= ~TIM2_SR1_UIF;
	// Enable capture/compare output
	TIM2->CCER1 = TIM2_CCER1_CC2E;
	// Setup PWM mode 1
	TIM2->CCMR2 = (0x06 << 4) | (1 << 3);
	// Set PWM mode data
	TIM2->CCR2H = 0x00;
	TIM2->CCR2L = 0x18;	
}

void irTxFrame(uint16_t pulseUs, uint16_t pauseUs, uint8_t *data, uint8_t bitCount){

	pIR_STATE st = &gIRState;
	// Don't send if no data to transmit
	if (bitCount < 32) return;
  // Check for currnet mode	
	if (st->mode == IR_MODE_RECEIVE){
		  // Disable GPIOA interrupt (IR Sensor)
			GPIOA->CR2 &= ~(1 << 3);
			// Stop timer
			TIM2->CR1  = 0;
			// Disable all timer interrups			
			TIM2->IER = 0;
			// Stop virtual timer
			vTimerStop(1);
	}
	// Copy frame to transmit
	st->tx.ir_pulse_time = pulseUs;
	st->tx.ir_pause_time = pauseUs;
	st->tx.tx_size = bitCount;
	memcpy(st->tx.ir_frame, data, ((bitCount + 7) >> 3));

	st->mode = IR_MODE_TRANSMIT;
	// Config Timer 2 to PWM - 38KHz
	configTimer2Tx();
	// Enable counter
  TIM2->CR1 |= TIM2_CR1_CEN;
	// Switch state to transmit pulse	
	st->tx.state = IR_TX_STATE_PULSE;
	// Tx preamble pulse
	TIM2->CCER1 |= TIM2_CCER1_CC2E;
	// Start timer for pulse time
	vTimerStart(1, st->tx.ir_pulse_time);	
}


void vTimer1Irq(void){
	pIR_STATE st = &gIRState;
  if (st->mode == IR_MODE_TRANSMIT){
			switch( st->tx.state ){
				
				case IR_TX_STATE_PULSE:
				{
					// Disable PWM
					TIM2->CCER1 &= ~TIM2_CCER1_CC2E;
					st->tx.state = IR_TX_STATE_PAUSE;
					vTimerStart(1, st->tx.ir_pause_time);
				}
				break;
				
				case IR_TX_STATE_PAUSE:
				{
					// Enable PWM
					TIM2->CCER1 |= TIM2_CCER1_CC2E;
					vTimerStart(1, IR_DATA_PULSE_TIME);
					st->tx.state = IR_TX_STATE_DATA_PULSE;
					st->tx.tx_ind = 0;
				}
				break;
				
				case IR_TX_STATE_DATA_PULSE:
				{
					// Disable PWM
					TIM2->CCER1 &= ~TIM2_CCER1_CC2E;
					
					if ( st->tx.tx_ind <= st->tx.tx_size ){
						// Calculate bit value
						uint8_t byte_ind = st->tx.tx_ind >> 3;
						uint8_t bit_ind  = st->tx.tx_ind & 0x07;
						uint8_t bit = ( st->tx.ir_frame[byte_ind] >> bit_ind ) & 0x01;
						// Depend on bit level generate appropiate pause
						if (bit){
							vTimerStart(1, IR_DATA_PAUSE_1_TIME);
						} else {
							vTimerStart(1, IR_DATA_PAUSE_0_TIME);
						}
						st->tx.state = IR_TX_STATE_DATA_PAUSE;
					} else {
						// It was last bit
						vTimerStart(1, IR_WAIT_AFTER_TX);
						st->tx.state = IR_TX_STATE_DATA_END;
					}
				}
				break;
				
				case IR_TX_STATE_DATA_PAUSE:
				{
					TIM2->CCER1 |= TIM2_CCER1_CC2E;
					vTimerStart(1, IR_DATA_PULSE_TIME);
					st->tx.state = IR_TX_STATE_DATA_PULSE;
					st->tx.tx_ind++;
				}
				break;
				
				case IR_TX_STATE_DATA_END:
				{
					// Stop transmit and swith to receive mode
					vTimerStop(1);
					st->mode = IR_MODE_RECEIVE;
					swithIrRxToIdle();
					startIrTimerRx();
					st->rx.ir_sz = 0;
				}
				break;
			}
		// End of transmit code
	} else if (st->mode == IR_MODE_RECEIVE){
		// Start receive code
		if (st->rx.state == IR_RX_STATE_RECV || st->rx.state == IR_RX_STATE_RECV_END){
			// Copy data
			if ( (st->rx.ir_ind > 31) && (st->rx.ir_sz == 0) ){
				memcpy(st->rx.ir_rdy_frame, st->rx.ir_frame, st->rx.ir_ind >> 3);
				st->rx.ir_sz = st->rx.ir_ind;
				st->rx.ir_ind = 0;
			}
			// Switch to idle state;			
			swithIrRxToIdle();
		}
	}	
}



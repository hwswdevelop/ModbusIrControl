/*	BASIC INTERRUPT VECTOR TABLE FOR STM8 devices
 *	Copyright (c) 2007 STMicroelectronics
 */
#include "stm8s.h"

typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector {
	unsigned char interrupt_instruction;
	interrupt_handler_t interrupt_handler;
};

@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}

extern void _stext();     /* startup routine */
extern INTERRUPT_HANDLER(CLK_IRQHandler, 4);
extern INTERRUPT_HANDLER(GPIOA_IntVector, 5);
extern INTERRUPT_HANDLER(TIM1_OvfIntVector, 13);
extern INTERRUPT_HANDLER(TIM1_CompIntVector, 14);
extern INTERRUPT_HANDLER(UART1_Tx_IRQHandler, 19);
extern INTERRUPT_HANDLER(UART1_Rx_IRQHandler, 20);

#if  0 

void irq(void){
	__asm(" nop");
}

INTERRUPT_HANDLER(Trap, 1){
	irq();
}
INTERRUPT_HANDLER(Irq0, 2){
	irq();
}
INTERRUPT_HANDLER(Irq1, 3){
	irq();
}
INTERRUPT_HANDLER(Irq4, 6){
	irq();
}
INTERRUPT_HANDLER(Irq5, 7){
	irq();
}
INTERRUPT_HANDLER(Irq6, 8){
	irq();
}
INTERRUPT_HANDLER(Irq7, 9){
	irq();
}
INTERRUPT_HANDLER(Irq8, 10){
	irq();
}
INTERRUPT_HANDLER(Irq9, 11){
	irq();
}
INTERRUPT_HANDLER(Irq10, 12){
	irq();
}
INTERRUPT_HANDLER(Irq12, 14){
	irq();
}
INTERRUPT_HANDLER(Irq14, 16){
	irq();
}
INTERRUPT_HANDLER(Irq15, 17){
	irq();
}
INTERRUPT_HANDLER(Irq16, 18){
	irq();
}
INTERRUPT_HANDLER(Irq19, 21){
	irq();
}
INTERRUPT_HANDLER(Irq20, 22){
	irq();
}
INTERRUPT_HANDLER(Irq21, 23){
	irq();
}
INTERRUPT_HANDLER(Irq22, 25){
	irq();
}
INTERRUPT_HANDLER(Irq23, 25){
	irq();
}
INTERRUPT_HANDLER(Irq24, 26){
	irq();
}
INTERRUPT_HANDLER(Irq25, 27){
	irq();
}
INTERRUPT_HANDLER(Irq26, 28){
	irq();
}
INTERRUPT_HANDLER(Irq27, 29){
	irq();
}
INTERRUPT_HANDLER(Irq28, 30){
	irq();
}
INTERRUPT_HANDLER(Irq29, 31){
	irq();
}


struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, Trap}, /* trap  */
	{0x82, Irq0}, /* irq0  */
	{0x82, Irq1}, /* irq1  */
	{0x82, CLK_IRQHandler}, /* irq2  */
	{0x82, GPIOA_IntVector}, /* irq3  */
	{0x82, Irq4}, /* irq4  */
	{0x82, Irq5}, /* irq5  */
	{0x82, Irq6}, /* irq6  */
	{0x82, Irq7}, /* irq7  */
	{0x82, Irq8}, /* irq8  */
	{0x82, Irq9}, /* irq9  */
	{0x82, Irq10}, /* irq10 */
	{0x82, TIM1_OvfIntVector}, /* irq11 */
	{0x82, TIM1_CompIntVector}, /* irq12 */
	{0x82, Irq13}, /* irq13 */
	{0x82, Irq14}, /* irq14 */
	{0x82, Irq15}, /* irq15 */
	{0x82, Irq16}, /* irq16 */
	{0x82, UART1_Tx_IRQHandler}, /* irq17 */
	{0x82, UART1_Rx_IRQHandler}, /* irq18 */
	{0x82, Irq19}, /* irq19 */
	{0x82, Irq20}, /* irq20 */
	{0x82, Irq21}, /* irq21 */
	{0x82, Irq22}, /* irq22 */
	{0x82, Irq23}, /* irq23 */
	{0x82, Irq24}, /* irq24 */
	{0x82, Irq25}, /* irq25 */
	{0x82, Irq26}, /* irq26 */
	{0x82, Irq27}, /* irq27 */
	{0x82, Irq28}, /* irq28 */
	{0x82, Irq29}, /* irq29 */
};


#else 

struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, NonHandledInterrupt}, /* trap  */
	{0x82, NonHandledInterrupt}, /* irq0  */
	{0x82, NonHandledInterrupt}, /* irq1  */
	{0x82, CLK_IRQHandler}, /* irq2  */
	{0x82, GPIOA_IntVector}, /* irq3  */
	{0x82, NonHandledInterrupt}, /* irq4  */
	{0x82, NonHandledInterrupt}, /* irq5  */
	{0x82, NonHandledInterrupt}, /* irq6  */
	{0x82, NonHandledInterrupt}, /* irq7  */
	{0x82, NonHandledInterrupt}, /* irq8  */
	{0x82, NonHandledInterrupt}, /* irq9  */
	{0x82, NonHandledInterrupt}, /* irq10 */
	{0x82, TIM1_OvfIntVector},   /* irq11 */
	{0x82, TIM1_CompIntVector},  /* irq12 */
	{0x82, NonHandledInterrupt}, /* irq13 */
	{0x82, NonHandledInterrupt}, /* irq14 */
	{0x82, NonHandledInterrupt}, /* irq15 */
	{0x82, NonHandledInterrupt}, /* irq16 */
	{0x82, UART1_Tx_IRQHandler}, /* irq17 */
	{0x82, UART1_Rx_IRQHandler}, /* irq18 */
	{0x82, NonHandledInterrupt}, /* irq19 */
	{0x82, NonHandledInterrupt}, /* irq20 */
	{0x82, NonHandledInterrupt}, /* irq21 */
	{0x82, NonHandledInterrupt}, /* irq22 */
	{0x82, NonHandledInterrupt}, /* irq23 */
	{0x82, NonHandledInterrupt}, /* irq24 */
	{0x82, NonHandledInterrupt}, /* irq25 */
	{0x82, NonHandledInterrupt}, /* irq26 */
	{0x82, NonHandledInterrupt}, /* irq27 */
	{0x82, NonHandledInterrupt}, /* irq28 */
	{0x82, NonHandledInterrupt}, /* irq29 */
};

#endif
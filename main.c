#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "queue.h"
#include "adc.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

unsigned int j;

void EXTI0_IRQHandler() {
    // Clear the interrupt
    Red_LED_On();
    EXTI->SWIER1 |= EXTI_SWIER1_SWI0;
}

int main(void){
	
	//struct queue q;
	uint8_t buffer[200];
	//uint16_t t;
	
	unsigned int n;
	//
	uint16_t i = 0;
	//
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	UART2_Init(); 
	
	//adc1Init();
	//
	//init_queue(&q);
	//
	//n = sprintf((char *)buffer, "Pushing elements\r\n");
    //USART_Write(USART2, buffer, n);    
	//
	//for (i = 0; i < 100; i++) {
    // push(&q,i);
	//}
	//
	//n = sprintf((char *)buffer, "Size %u\r\n", elementsInQueue(&q));
    //USART_Write(USART2, buffer, n); 
	//
	//n = sprintf((char *)buffer, "Size %u\r\n", elementsInQueue(&q));
    //USART_Write(USART2, buffer, n); 
	//
    //t = elementsInQueue(&q);
	//
	//for (i = 0; i < t; i++) {
	//    	n = sprintf((char *)buffer, "Data %u\r\n", pop(&q));
    //    USART_Write(USART2, buffer, n); 
	//}
    
    adcInit();
    adcSelfTest();
    adcInitChannel(5);
	  n = sprintf((char *)buffer, "Is empty %u\r\n", getNumReadingsInFIFO(5));
    USART_Write(USART2, buffer, n); 
    adcInterruptOn();
    setInterruptWaterline(5,1);
    startConversion();
	
	
	while (1){       
	}
}

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

void adc1Init() {
	
	unsigned int i;
	
	// Enable ADCEN bit 13 of AHB2 clock enable register
	RCC->AHB2ENR |= (1 << 13);
	// ADC starts in deep power down to reduce leakage current and save power
	ADC1->CR &= ~ADC_CR_DEEPPWD;
	// Enable the ADC internatl voltage regulator
	ADC1->CR |= ADC_CR_ADVREGEN;
	
	// ADC voltage regulator start-up time is 20 us max
	// Do we need to wait here?
	for (i = 0; i < 10000000; i++) {
		j += 1;
	}
	
	ADC123_COMMON->CCR = (1 << 16);
	
	// Configure for Single ended input calibration
	ADC1->CR &= ~ADC_CR_ADCALDIF;
	// Initalize ADC Channel 1 Calibration
	ADC1->CR |= ADC_CR_ADCAL;
	// Wait for calibration to complete
		for (i = 0; i < 10000000; i++) {
		j += 1;
	}
  RCC->AHB2ENR |= 1;
	GPIOA->ASCR |= 1;//0xffff;
	ADC1->SQR1 |= 0x140;
	ADC1->CR |= 1;
	//TSC->IOASCR = 1;
	while((ADC1->CR & ADC_CR_ADCAL) != 0);
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
    adcInitChannel(6);
	n = sprintf((char *)buffer, "Is empty %u\r\n", getNumReadingsInFIFO(5));
    USART_Write(USART2, buffer, n); 
    startConversion();
    for (i = 0; i < 10000; i++) {};
	n = sprintf((char *)buffer, "Is empty %u\r\n", getNumReadingsInFIFO(5));
    USART_Write(USART2, buffer, n);		
	 	n = sprintf((char *)buffer, "Data %u\r\n", getData(5));
    USART_Write(USART2, buffer, n);		
	
	
	while (1){       
	}
}

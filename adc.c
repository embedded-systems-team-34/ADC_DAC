/******************************************************************************
* FILENAME : adc.c      
*
* DESCRIPTION : 
*     adc implementation
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/

#include "adc.h"

void adcInit() {
    
    unsigned int i,j;
    
    // Enable ADCEN bit 13 of AHB2 clock enable register
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    
    // ADC starts in deep power down to reduce leakage current and save power
    // Clear this bit to exit deep-power down
	ADC1->CR &= ~ADC_CR_DEEPPWD;
    // Enable the ADC internatl voltage regulator
	ADC1->CR |= ADC_CR_ADVREGEN;
    
    // According to refernce manual ADC voltage regulator start-up time is 20 us max
    // Spin for a while to ensure we do not enable the ADC until this time has expired
	for (i = 0; i < 10000000; i++) {
		j += 1;
	}
    
    // Select sychronous clock mode HCLK/1 
	ADC123_COMMON->CCR = ADC_CCR_CKMODE_0;  

	// Configure for Single ended input calibration
	ADC1->CR &= ~ADC_CR_ADCALDIF;    

    // Initalize ADC Channel 1 Calibration
	ADC1->CR |= ADC_CR_ADCAL;
    
    // Wait for calibration to complete
    while((ADC1->CR & ADC_CR_ADCAL) != 0);
    ADC1-CR |= ADC_CR_ADEN;
    
}

unsigned int selfTest(void) {
    // Enable the reference voltage
    ADC123_COMMON->CCR |= ADC_CCR_VREFEN;
    // Set channel 0, with a channel length of 1
    ADC1->SQR1 = 0;
    // Start a conversion on channel 0
    ADC1->ADC_CR_ADSTART;
    // Poll the EOS bit to wait until the conversion is complete
    while ((ADC1->ISR & ADC_ISR_EOS) != ADC_ISR_EOS);
    
    // Verify that VREFEN is as expected
    
    
}
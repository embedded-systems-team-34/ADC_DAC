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

#if DEBUG
uint8_t buffer[200];
unsigned int buf_size;
#endif

struct queue q[16];

// ADC1 Interrupt Handler
void ADC1_2_IRQHandler() {
    unsigned int whichInterrupt = ADC1->ISR;
    // End of sampling flag, set by completion of conversion of any channel at the end of sampling phase
    if ((whichInterrupt & ADC_ISR_EOSMP) != 0) {
        
    }
}

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
    ADC1->CR |= ADC_CR_ADEN;
    
    adcInitQueues();
    
    NVIC_EnableIRQ(ADC1_2_IRQn);
    // Enable end of sampling flag interrupt
    ADC1->IER |= ADC_IER_EOSMP;
    
}

unsigned int adcSelfTest(void) {
    
    unsigned int i;
    unsigned int measured_vref;
    unsigned int int_mask;
    
    // Determine active interrupts so we can mask them back on after selfTest
    int_mask = ADC1->IER;
    // Mask of all interupts in selfTest Mode
    ADC1->IER = 0;
    
    // Enable the reference voltage
    ADC123_COMMON->CCR |= ADC_CCR_VREFEN;
    // Give the reference some time to come up and stabalize
    for (i = 0; i < 10000000; i++) {};
    // Set channel 0, with a channel length of 1
    ADC1->SQR1 = 0;
    // Start a conversion on channel 0
    ADC1->CR |= ADC_CR_ADSTART;
    // Poll the EOS bit to wait until the conversion is complete
    while ((ADC1->ISR & ADC_ISR_EOS) != ADC_ISR_EOS);
    
    // Verify that VREFEN is as expected
    
    measured_vref = ADC1->DR;
    
#if DEBUG
    buf_size = sprintf((char *)buffer, "VREFEN reading %u\r\n", ADC1->DR);
    USART_Write(USART2, buffer, buf_size);
#endif
    
    ADC1->IER = int_mask;
    if (measured_vref > VREF_MIN_THRESHOLD) {
#if DEBUG
        buf_size = sprintf((char *)buffer, "Self Test Successful\r\n");
        USART_Write(USART2, buffer, buf_size);
#endif
        return 1;
    } else {
#if DEBUG
        buf_size = sprintf((char *)buffer, "Self Test FAIL\r\n");
        USART_Write(USART2, buffer, buf_size);
#endif
        return 0;
    }    
}

void adcInitQueues(void) {
    
    unsigned int i = 0;
    
    // Initalize the queues, one for each channel
    for (i = 0; i < NUM_CHANNELS; i++) {
        init_queue(&q[i]);
    }
}

unsigned int isFIFOEmpty(unsigned int channel) {
    // If valid channel then get number entries of FIFO
    if (channel < NUM_CHANNELS) {
        return (isEmpty(&q[channel]));
    }
    return 1; 
}

unsigned int getNumReadingsInFIFO(unsigned int channel) {
    // If valid channel then get number entries of FIFO
    if (channel < NUM_CHANNELS) {
        return (elementsInQueue(&q[channel]));
    }
    return 1;
}

// Set ADC to contionusly convert the currently configured channels 
void adcSetModeContinous(void) {}

// Set ADC to convert the currently confiugred channels for a single cycle
void adcSetModeSingle(void) {}

// returns 1 for continous operation, 0 for single
unsigned int adcGetMode(void) {}

// Perform necessary setup to setup specific channel for conversion
// chan - channel to enable
// bit 1 to channel 1 etc.
void adcInitChannel(unsigned int chan) {
    // Turn on CLK for Port channel is located on
    
    // Close analog switch control for given port
    
    // Set sequencing for all channels that are on
}

// Start a conversion with the currently set mode
void startConversion(void) {}

// Get most recent adc conversion value for a specified channel samples 
unsigned int getData(unsigned int channel) {
    // If valid channel then get number entries of FIFO
    if (channel < NUM_CHANNELS) {
        return (pop(&q[channel]));
    }
    return 0;    
}

// Turn on interrupt notification mode
unsigned int adcInterruptOn(void);

// Turn off interrupt notification mode
unsigned int adcInterruptsOff(void);

// Set the number of conversion results for which to generate an interrupt
unsigned int setInterruptWaterline(unsigned int channel, unsigned int waterline);

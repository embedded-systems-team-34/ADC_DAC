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
struct adc a; 

// ADC1 Interrupt Handler
void ADC1_2_IRQHandler() {
    unsigned int whichInterrupt = ADC1->ISR;
    // Channel of queue to store the ADC result in
    unsigned int queue_channel;
    
    Green_LED_On();
    // End of conversion flag, set by completion of conversion of any channel at the end 
    if ((whichInterrupt & ADC_ISR_EOC) != 0) {
        queue_channel = a.currentActiveChannel;
        // Push the sampled data into the queue
        push(&q[queue_channel], ADC1->DR);
        // Get the next channel to convert
        getNextActiveChannel();
    }
}

void adcInit() {
    
    unsigned int i,j;
    
    initADCStruct();
    
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
    ADC1->IER |= (ADC_ISR_EOC);
    
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
    ADC1->ISR |= ADC_ISR_EOS;
    
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
void adcSetModeContinous(void) {
    a.mode = CONTINOUS;
}

// Set ADC to convert the currently confiugred channels for a single cycle
void adcSetModeSingle(void) {
    a.mode = SINGLE;
}

// returns 1 for continous operation, 0 for single
conversion_rate_t adcGetMode(void) {
    return a.mode;
}

// Perform necessary setup to setup specific channel for conversion
// chan - channel to enable
// bit 1 to channel 1 etc.
void adcInitChannel(unsigned int channel) {
    
    unsigned int i = 0;
    unsigned int channel_loop_count = 0;
    
    if (channel < NUM_CHANNELS) { 
        
        a.channel_enable[channel] = 1;
        if (a.numActiveChannels != NUM_CHANNELS) {
            a.numActiveChannels += 1;
        }
        
        // Turn on CLK for Port channel is located on
        // PORTC Channels
        if (((channel > 0) && (channel <= 4)) || (channel == 13) || (channel == 14)) {
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
        // PORTA
        } else if (channel >= 5 && channel <= 12) {
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
        // PORTB
        } else if ((channel == 15) || (channel == 16)) {
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
        }
        
        // Close analog switch control for given port
        switch(channel) {
            case 0:
                break;
            case 1:
                GPIOC->ASCR |= 1;
                break;
            case 2:
                GPIOC->ASCR |= 2;
                break;
            case 3:
                GPIOC->ASCR |= 4;
                break;
            case 4:
                GPIOC->ASCR |= 8;
                break;
            case 5:
                GPIOA->ASCR |= 1;
                break;
            case 6:
                GPIOA->ASCR |= 2;
                break;
            case 7:
                GPIOA->ASCR |= 4;
                break;
            case 8:
                GPIOA->ASCR |= 8;
                break;
            case 9:
                GPIOA->ASCR |= 16;
                break;
            case 10:
                GPIOA->ASCR |= 32;
                break;
            case 11:
                GPIOA->ASCR |= 64;
                break;
            case 12:
                GPIOA->ASCR |= 128;
                break;
            case 13:
                GPIOC->ASCR |= 16;
                break;
            case 14:
                GPIOC->ASCR |= 32;
                break;
            case 15:
                GPIOB->ASCR |= 1;
                break;
            case 16:
                GPIOB->ASCR |= 2;
                break;
            
        }
        
        if (a.numActiveChannels != 0) {
            for (i = 0; i < NUM_CHANNELS; i++) {
                if (a.channel_enable[i] == 1) {
                    writeSQRRegister(i, channel_loop_count);
                    channel_loop_count += 1;
                }
            }
            // Set sequencing for all channels that are on
            ADC1->SQR1 |= (a.numActiveChannels-1 & ADC_SQR1_L);
        }
    }
}

// Start a conversion with the currently set mode
void startConversion(void) {
    
    unsigned int i = 0;
    
    // Get the first active channel
    for (i = 0; i < NUM_CHANNELS; i++) {
        if (a.channel_enable[i] == 1) {
            a.currentActiveChannel = i;
            break;
        }
    }
    
    if (a.mode == SINGLE) {
        ADC1->CR |= ADC_CR_ADSTART;    
    // Must be CONTINOUS Mode    
    } else {
        
    } 
}

// Get most recent adc conversion value for a specified channel samples 
unsigned int getData(unsigned int channel) {
    // If valid channel then get number entries of FIFO
    if (channel < NUM_CHANNELS) {
        return (pop(&q[channel]));
    }
    return 0;    
}

void initADCStruct() {
    unsigned int i = 0;
    
    a.mode = SINGLE;
    a.numActiveChannels = 0;
    a.currentActiveChannel = 0;
    
    for ( i = 0; i < NUM_CHANNELS; i++) {
        // Initalize all channels to off
        a.channel_enable[i] = 0;
        // Default all waterlines to 1
        a.waterline_channel[i] = 1;        
    }
}

// Turn on interrupt notification mode
unsigned int adcInterruptOn(void);

// Turn off interrupt notification mode
unsigned int adcInterruptsOff(void);

// Set the number of conversion results for which to generate an interrupt
void setInterruptWaterline(unsigned int channel, unsigned int waterline) {
    if (channel < NUM_CHANNELS) {
        a.waterline_channel[channel] = waterline;
    }
}

void writeSQRRegister(unsigned int channel, unsigned int pos) {
    switch (pos) {
    
        case 0:
            ADC1->SQR1 |= ((channel << 6) & ADC_SQR1_SQ1);
            break;
        case 1:
            ADC1->SQR1 |= ((channel << 12) & ADC_SQR1_SQ2);
            break;
        case 2:
            ADC1->SQR1 |= ((channel << 18) & ADC_SQR1_SQ3);
            break;
        case 3:
            ADC1->SQR1 |= ((channel << 24) & ADC_SQR1_SQ4);
            break;            
        case 4:
            ADC1->SQR2 |= ((channel) & ADC_SQR2_SQ5);
            break;
        case 5:
            ADC1->SQR2 |= ((channel << 6) & ADC_SQR2_SQ6);
            break;      
        case 6:
            ADC1->SQR2 |= ((channel << 12) & ADC_SQR2_SQ7);
            break; 
        case 7:
            ADC1->SQR2 |= ((channel << 18) & ADC_SQR2_SQ8);
            break;        
        case 8:
            ADC1->SQR2 |= ((channel << 24) & ADC_SQR2_SQ9);
            break;              
        case 9:
            ADC1->SQR3 |= ((channel) & ADC_SQR3_SQ10);
            break;     
        case 10:
            ADC1->SQR3 |= ((channel << 6) & ADC_SQR3_SQ11);
            break; 
        case 11:
            ADC1->SQR3 |= ((channel << 12) & ADC_SQR3_SQ12);
            break;     
        case 12:
            ADC1->SQR3 |= ((channel << 18) & ADC_SQR3_SQ13);
            break;      
        case 13:
            ADC1->SQR3 |= ((channel << 24) & ADC_SQR3_SQ14);
            break;      
        case 14:
            ADC1->SQR4 |= ((channel) & ADC_SQR4_SQ15);
            break;   
        case 15:
            ADC1->SQR4 |= ((channel << 6) & ADC_SQR4_SQ16);
            break;
    }            
}

unsigned int getNextActiveChannel(void) {
    
    unsigned int i = 0;
    unsigned int next_index = 0;
    
    // If only one active channel then next channel is current channel
    if (a.numActiveChannels == 1) {
        return a.currentActiveChannel;
    // Else have more than one channel so determine next channel in the sequence
    } else {
        for (i = 1; i < NUM_CHANNELS; i++) {
            next_index = (i + a.currentActiveChannel) % NUM_CHANNELS;
            if (a.channel_enable[next_index] == 1) {
                a.currentActiveChannel = next_index;
                return a.currentActiveChannel;
            }
        }
    }
    return 0;
}

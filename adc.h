/******************************************************************************
* FILENAME : adc.h          
*
* DESCRIPTION : 
*     adc function prototypes
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/
#ifndef ADC_H
#define ADC_H

#define DEBUG (1)

#include "stm32l476xx.h"

#if DEBUG
#include "UART.h"
#include <stdio.h>
#include "led.h"
#endif

#include "queue.h"

#define VREF_MIN_THRESHOLD (400)
#define NUM_CHANNELS (17)

/*******************************************************************************
--------------------------------------------------------------------------------
ADC1 Channel MAP
--------------------------------------------------------------------------------
ADC123_IN0  - VREF - NA 
ADC123_IN1  - PC0  - MEMS
ADC123_IN2  - PC1  - MEMS
ADC123_IN3  - PC2  - MEMS
ADC123_IN4  - PC3  - MEMS
ADC123_IN5  - PA0  - JOY_CENTER
ADC123_IN6  - PA1  - JOY_LEFT
ADC123_IN7  - PA2  - JOY_RIGHT
ADC123_IN8  - PA3  - JOY_UP
ADC123_IN9  - PA4  - 
ADC123_IN10 - PA5  - JOY_DOWN
ADC123_IN11 - PA6  - LCD
ADC123_IN12 - PA7  - LCD
ADC123_IN13 - PC4  - LCD
ADC123_IN14 - PC5  - LCD
ADC123_IN15 - PB0  - LCD
ADC123_IN16 - PB1  - LCD
*******************************************************************************/

typedef enum {
    SINGLE,
    CONTINOUS
} conversion_rate_t;

typedef enum {
    INTERRUPTS_ON,
    INTERRUPTS_OFF
} interrupts_t;

struct adc {
    // 1 if a channel is enabled, else 0
    unsigned int channel_enable[NUM_CHANNELS];
    // Conversion Rate for ADC1
    conversion_rate_t mode;
    // Waterline per channel which determines when interrupt is triggered
    unsigned int waterline_channel[NUM_CHANNELS];
    unsigned int numActiveChannels;
    unsigned int currentActiveChannel;
    interrupts_t interruptsOn;
};

// Measure Vrefint to determine if ADC is operational
// returns - 1 self test pass, 0 self test fails 
unsigned int adcSelfTest(void);

// Initalization ADC1, turn ADC1 on and perform calibration
void adcInit(void);

// Setup the FIFOs for data storage on all channels
void adcInitQueues(void);

// Set ADC to contionusly convert the currently configured channels 
void adcSetModeContinous(void);

// Set ADC to convert the currently confiugred channels for a single cycle
void adcSetModeSingle(void);

// returns 1 for continous operation, 0 for single
conversion_rate_t adcGetMode(void);

// Perform necessary setup to setup specific channel for conversion
// chan - mask of channels that are enabled where bit 0 corresponds to channel 0, 
// bit 1 to channel 1 etc.
void adcInitChannel(unsigned int channel);

// Start a conversion with the currently set mode
void startConversion(void);

// Get most recent adc conversion value for a specified channel samples 
unsigned int getData(unsigned int channel);

// Get number of elements waiting to be read from the queue for a specified channel
unsigned int getNumReadingsInFIFO(unsigned int channel);

// Return 1 iff FIFO is empty, else return 0 if there is at least 1 result to read
unsigned int isFIFOEmpty(unsigned int channel);

// Turn on interrupt notification mode
void adcInterruptOn(void);

// Turn off interrupt notification mode
void adcInterruptsOff(void);

// Set the number of conversion results for which to generate an interrupt
void setInterruptWaterline(unsigned int channel, unsigned int waterline);

void initADCStruct(void);

// Write the SQR register
// channel - channel to write 
// pos - the position of the channel in the sequence
void writeSQRRegister(unsigned int channel, unsigned int pos);

// Determine the next active channel to decode which channel a specific EOC interrupt pertains to
unsigned int getNextActiveChannel(void);

#endif

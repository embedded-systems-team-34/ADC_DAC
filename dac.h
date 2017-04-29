/******************************************************************************
* FILENAME : dac.h          
*
* DESCRIPTION : 
*     daac function prototypes
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/
#ifndef DAC_H
#define DAC_H

#include "stm32l476xx.h"

// DAC1_OUT1 - PA4
// DAC1_OUT2 - PA5

void dacInit(void);

void singleConversion(unsigned int channel, uint16_t data);

#endif

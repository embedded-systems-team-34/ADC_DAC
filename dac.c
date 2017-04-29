/******************************************************************************
* FILENAME : dac.c      
*
* DESCRIPTION : 
*     dac implementation
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/

#include "dac.h"

void dacInit(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
}

void singleConversion(unsigned int channel, uint16_t data) {
    if (channel == 1) {
        DAC->CR |= DAC_CR_EN1;    
        DAC->DHR12R1 = data;
			  DAC->CR |= DAC_CR_TEN1;
    } 
    if (channel == 2) {
        DAC->CR |= DAC_CR_EN2;
        DAC->DHR12R2 = data;
			  DAC->CR |= DAC_CR_TEN2;
    }
}

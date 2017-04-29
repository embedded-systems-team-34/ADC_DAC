/******************************************************************************
* FILENAME : timer.c      
*
* DESCRIPTION : 
*     timer2 implementation
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/

#include "timer2.h"
#include "dac.h"

extern uint16_t cos_lookup_table[100];
extern uint16_t square_lookup_table[100];
extern uint16_t sawtooth_lookup_table[100];

void TIM2_IRQHandler(void) {
    uint16_t which_interrupt = TIM2->SR;
    static uint16_t i = 0;
	  uint16_t data;
    
    
    // Check for overflow interrupt
    if (((which_interrupt & TIM_SR_UIF) == TIM_SR_UIF)) {
	    data = sawtooth_lookup_table[i];//square_lookup_table[i];//cos_lookup_table[i];
        singleConversion(2,data);
        i += 1;
        i = i % 100;
        TIM2->SR &= ~TIM_SR_UIF; // Clear overflow interrupt
    }
}

// interrupt_period - Count of 50 us resolution of interrupt period -> 20 represents 1 ms (20 * 50 us) = 1 ms
void configureSystemTick(uint16_t interrupt_period) {
    // Enable the interrupt handler
    NVIC_EnableIRQ(TIM2_IRQn); 
    
    // Enable clock of timer 2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    // Set up TIM2 to generate system tick at 100 ms 
    
    // Set Prescaler
    // 80 MHz / 4000 = 20 KHz -> 50 us
    TIM2->PSC = 3999;
    
    TIM2->ARR = 1;
    TIM2->EGR |= TIM_EGR_UG;
    
    // Unmask TIM2 Interrupts
    TIM2->DIER |= TIM_DIER_UIE;
    
    TIM2->CR1 |= TIM_CR1_CEN;
}


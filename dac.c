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
#include "queue.h"

struct queue dac_q;
struct dac d; 

void TIM2_IRQHandler(void) {
    uint16_t which_interrupt = TIM2->SR;
	uint16_t data;
    
    if (d.conversion_rate == DAC_CONTINOUS) {
        // Check for overflow interrupt
        if (((which_interrupt & TIM_SR_UIF) == TIM_SR_UIF)) {
            if (d.data_source == FIXED) {
                data = d.fixedDataSoruce_ptr[d.fixed_data_source_index];
                d.fixed_data_source_index += 1;
                d.fixed_data_source_index = d.fixed_data_source_index % d.fixedDataSourceLength;
            } else {
                data = pop(&dac_q);
            }
            singleConversion(data);
            TIM2->SR &= ~TIM_SR_UIF; // Clear overflow interrupt
        }
    }
}

void dacInit(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
    initDacStruct();
}

void initDacStruct() {
    d.conversion_rate = DAC_SINGLE;
    d.data_source = FIXED;
    d.fixedDataSourceLength = 0;
    d.sample_period = 1;
    d.fixed_data_source_index = 0;
    d.active_channel = 2;
}

void setDacActiveChannel(unsigned int channel) {
    if (channel == 1) {
        d.active_channel = 1;
    } 
    if (channel == 2) {
        d.active_channel = 2;
    }    
}

void singleConversion(uint16_t data) {
    if (d.active_channel == 1) {
        DAC->CR |= DAC_CR_EN1;    
        DAC->DHR12R1 = data;
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    } 
    if (d.active_channel == 2) {
        DAC->CR |= DAC_CR_EN2;
        DAC->DHR12R2 = data;
		DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;
    }
}

void dacInitQueue(void) {
    init_queue(&dac_q);
}

void writeDacOutputData(uint16_t data) {
    push(&dac_q, data);
}

void setDacContinous(uint16_t sampling_period) {
    d.conversion_rate = DAC_CONTINOUS;  
    d.sample_period = sampling_period;
    configureDacContinousMode(sampling_period);    
}

void setDacSingle() {
    d.conversion_rate = DAC_SINGLE;  
    TIM2->DIER &= ~TIM_DIER_UIE;
}
void setDacFixedDataSource(uint16_t *datasource_ptr, uint16_t length) {
    d.fixedDataSourceLength = length;
    d.fixedDataSoruce_ptr = datasource_ptr;
    d.data_source = FIXED;
}

void setDacQueueDataSource() {
    d.data_source = DYNAMIC_FIFO;
}

// interrupt_period - Count of 50 us resolution of interrupt period -> 20 represents 1 ms (20 * 50 us) = 1 ms
void configureDacContinousMode(uint16_t interrupt_period) {
    // Enable the interrupt handler
    NVIC_EnableIRQ(TIM2_IRQn); 
    
    // Enable clock of timer 2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    // Set Prescaler
    // 80 MHz / 4000 = 20 KHz -> 50 us
    TIM2->PSC = 3999;
    
    TIM2->ARR = d.sample_period-1;
    TIM2->EGR |= TIM_EGR_UG;
    
    // Unmask TIM2 Interrupts
    TIM2->DIER |= TIM_DIER_UIE;
    
    TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t getSamplesInDacQueue(void) {
    return elementsInQueue(&dac_q);
}

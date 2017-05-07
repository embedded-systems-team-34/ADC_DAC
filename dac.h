/******************************************************************************
* FILENAME : dac.h          
*
* DESCRIPTION : 
*     DAC function prototypes
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

typedef enum {
    DAC_SINGLE,
    DAC_CONTINOUS
} dac_conversion_rate_t;

typedef enum {
    FIXED,
    DYNAMIC_FIFO
} data_source_t;

struct dac {
    dac_conversion_rate_t conversion_rate;
    data_source_t data_source;
    uint16_t *fixedDataSoruce_ptr;
    uint16_t fixedDataSourceLength;
    // Sample period measured with 50 us resolution
    uint16_t sample_period;
    uint16_t fixed_data_source_index;
    uint16_t active_channel;
};

// DAC Interface Functions
void dacInit(void);
void singleConversion(uint16_t data);
void writeDacOutputData(uint16_t data);
void setDacContinous(uint16_t sampling_period);
void setDacSingle(void);
void setDacFixedDataSource(uint16_t *datasource_ptr, uint16_t length);
void setDacQueueDataSource(void);
void setDacActiveChannel( unsigned int channel);
void configureDacContinousMode(void);
uint16_t getSamplesInDacQueue(void);

// Helper Functions
void initDacStruct(void);
void dacInitQueue(void);


#endif

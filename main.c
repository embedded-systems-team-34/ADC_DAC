#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "queue.h"
#include "adc.h"
#include "dac.h"

#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#define NUM_ELEMENTS (100)


// 100 Hz sawtooth wave sampled at 10 kHz
uint16_t sawtooth_lookup_table[NUM_ELEMENTS] = {
40,
81,
122,
163,
204,
245,
286,
327,
368,
409,
450,
491,
532,
573,
614,
655,
696,
737,
778,
819,
859,
900,
941,
982,
1023,
1064,
1105,
1146,
1187,
1228,
1269,
1310,
1351,
1392,
1433,
1474,
1515,
1556,
1597,
1638,
1678,
1719,
1760,
1801,
1842,
1883,
1924,
1965,
2006,
2047,
2088,
2129,
2170,
2211,
2252,
2293,
2334,
2375,
2416,
2457,
2497,
2538,
2579,
2620,
2661,
2702,
2743,
2784,
2825,
2866,
2907,
2948,
2989,
3030,
3071,
3112,
3153,
3194,
3235,
3276,
3316,
3357,
3398,
3439,
3480,
3521,
3562,
3603,
3644,
3685,
3726,
3767,
3808,
3849,
3890,
3931,
3972,
4013,
4054,
4095};


// 100 Hz square wave sampled at 10 kHz
uint16_t square_lookup_table[NUM_ELEMENTS] = {
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
4095,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0};

// cosine signal with a frequency of 100 Hz when sampled at 10 kHz
uint16_t cos_lookup_table[NUM_ELEMENTS] = {
4090,
4078,
4058,
4030,
3994,
3951,
3900,
3841,
3776,
3703,
3625,
3540,
3449,
3352,
3250,
3144,
3033,
2919,
2801,
2680,
2556,
2431,
2304,
2176,
2047,
1918,
1790,
1663,
1538,
1414,
1293,
1175,
1061,
950,
844,
742,
645,
554,
469,
391,
318,
253,
194,
143,
100,
64,
36,
16,
4,
0,
4,
16,
36,
64,
100,
143,
194,
253,
318,
391,
469,
554,
645,
742,
844,
950,
1061,
1175,
1293,
1414,
1538,
1663,
1790,
1918,
2047,
2176,
2304,
2431,
2556,
2680,
2801,
2919,
3033,
3144,
3250,
3352,
3449,
3540,
3625,
3703,
3776,
3841,
3900,
3951,
3994,
4030,
4058,
4078,
4090,
4095};

unsigned int randomNum(unsigned int min, unsigned int max) {
    return rand() % (max + 1 - min) + min;
}

uint16_t lut_index = 0;

int main(void){
    
    uint16_t dac_data = 0;
    

	System_Clock_Init(); // Switch System Clock = 80 MHz
/**********************************************************************************************
START DAC TEST CODE 
**********************************************************************************************/	
// Test Single Mode
#if 0
	dacInit(); 
    setDacSingle();
    setDacActiveChannel(2);
    singleConversion(0x3ff);
#endif
	
// Test Continous Mode cos
#if 1
	dacInit(); 
    // Select the cosine waveform
    setDacFixedDataSource(cos_lookup_table, NUM_ELEMENTS);
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2);
#endif

// Test Continous Mode sawtooth
#if 0
	dacInit(); 
    // Select the sawtooth waveform
    setDacFixedDataSource(sawtooth_lookup_table, NUM_ELEMENTS);
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2);
#endif

// Test Continous Mode square
#if 0
    dacInit(); 
    // Select the square waveform
    setDacFixedDataSource(square_lookup_table, NUM_ELEMENTS);
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2);
#endif

// Test Dynamic FIFO Constant Source
#if 0
    // Give the DAC an initial value
    writeDacOutputData(0xfff);
    dacInit(); 
    // Select the square waveform
    setDacQueueDataSource();
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2); 
    // Feed the DAC FIFO 
    while(1) {
        // If the samples in the DAC queue are less than 10, feed it to prevent DAC fifo from becoming empty
        if (getSamplesInDacQueue() <= 10) {
            writeDacOutputData(0xfff);
        }
    }
#endif

// Test Dynamic FIFO Increasing Source
// Generate a Triangle Wave at 2.5 Hz as a new sample is output every 100 us and there are 4095 unique samples -> 4095 * 100us roughly 400 ms
#if 0
    // Give the DAC an initial value
    writeDacOutputData(dac_data);
    dacInit(); 
    // Select the square waveform
    setDacQueueDataSource();
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2); 
    // Feed the DAC FIFO 
    while(1) {
        // If the samples in the DAC queue are less than 10, feed it to prevent DAC fifo from becoming empty
        if (getSamplesInDacQueue() <= 10) {
            dac_data += 1;
            dac_data %= 0xFFF;
            writeDacOutputData(dac_data);
        }
    }
#endif

// Test Dynamic FIFO with AWGN Random Noise Source
#if 0
    // Give the DAC an initial value
    writeDacOutputData(dac_data);
    dacInit(); 
    // Select the square waveform
    setDacQueueDataSource();
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2); 
    // Feed the DAC FIFO 
    while(1) {
        // If the samples in the DAC queue are less than 10, feed it to prevent DAC fifo from becoming empty
        if (getSamplesInDacQueue() <= 10) {
            dac_data = randomNum(1,0xFFF);
            writeDacOutputData(dac_data);
        }
    }
#endif

// Test Dynamic FIFO with AWGN Random Noise Source mixed with cos waveform
#if 1
    // Give the DAC an initial value
    writeDacOutputData(dac_data);
    dacInit(); 
    // Select the square waveform
    setDacQueueDataSource();
    setDacActiveChannel(2);  
    // Specify a sample rate of 10 khz -> 100 us (2 * 50 us)
    setDacContinous(2); 
    // Feed the DAC FIFO 
    while(1) {
        // If the samples in the DAC queue are less than 10, feed it to prevent DAC fifo from becoming empty
        if (getSamplesInDacQueue() <= 10) {
            dac_data = randomNum(1,500);
            dac_data += (cos_lookup_table[lut_index] / 2) + 0x3ff;
            lut_index += 1;
            lut_index %= NUM_ELEMENTS;
            writeDacOutputData(dac_data);
        }
    }
#endif

/**********************************************************************************************
END DAC TEST CODE 
**********************************************************************************************/	
    
	while (1){   
	}
}

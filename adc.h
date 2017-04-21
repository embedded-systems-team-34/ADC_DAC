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

#define DEBUG (1)

// Measure Vrefint to determine if ADC is operational
// returns - 1 self test pass, 0 self test fails 
unsigned int selfTest(void);

// Initalization ADC1, turn ADC1 on and perform calibration
void adcInit();

void adcConfigureChannel();
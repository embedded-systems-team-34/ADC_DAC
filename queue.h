/******************************************************************************
* FILENAME : queue.h          
*
* DESCRIPTION : 
*     Queue function prototypes
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/
#ifndef QUEUE_H
#define QUEUE_H

#include "stm32l476xx.h"

#define MAX_QUEUE_SIZE (200)

struct queue {
	uint16_t buffer[MAX_QUEUE_SIZE];
	uint16_t in_ptr;
	uint16_t out_ptr;
	uint16_t length;
};

void init_queue(struct queue * q);
uint16_t isEmpty(struct queue* q);
uint16_t elementsInQueue(struct queue* q);
uint16_t push(struct queue* q, uint16_t val);
uint16_t pop(struct queue* q) ;

#endif

/******************************************************************************
* FILENAME : queue.c      
*
* DESCRIPTION : 
*     Queue implementation
*
* AUTHOR: 
*     Donald MacIntyre - djm4912@rit.edu
*
******************************************************************************/

#include "queue.h"

void init_queue(struct queue* q) {
  q->in_ptr = 0;
  q->out_ptr = 0;
  q->length = 0;
}

uint16_t isEmpty(struct queue* q) {
    if (q->length == 0) {
        return 1;
    } else {
        return 0;
    }
}

uint16_t elementsInQueue(struct queue* q) {
    return q->length;
}

// Push an element onto the queue and return TRUE iff the push is successful
uint16_t push(struct queue* q, uint16_t val) {
    // First check that Queue is not full 
    if (q->length == MAX_QUEUE_SIZE) {
        return 0;
    }
    // Queue is not full so add the customer
    q->buffer[q->in_ptr] = val;
    q->in_ptr = (q->in_ptr + 1) % MAX_QUEUE_SIZE; 
    q->length += 1;
    return 1;
}

//Pop the first element from the queue
uint16_t pop(struct queue* q) {
	  uint16_t val = 0;
    val = q->buffer[q->out_ptr];
    q->out_ptr = (q->out_ptr + 1) % MAX_QUEUE_SIZE; 
    q->length -= 1;
    return val;
    
}

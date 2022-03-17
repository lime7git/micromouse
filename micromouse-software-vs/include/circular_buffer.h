#ifndef _CIRCULAR_BUFFER_H
#define _CIRCULAR_BUFFER_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"
#include <stdlib.h>

typedef struct {
    uint8_t *buffer;
    int head;
    int tail;
    int size;
		int commands_cnt;
} tCircular_buffer;

typedef enum {
	BUFFER_SUCCESS 	= 0,
	BUFFER_FULL 		= 1,
	BUFFER_EMPTY 		= 2
} eCircular_buffer_status;

eCircular_buffer_status tCircular_buffer_init(tCircular_buffer *hBuffer, uint16_t size);
eCircular_buffer_status tCircular_buffer_push(tCircular_buffer *hBuffer, uint8_t data);
eCircular_buffer_status tCircular_buffer_pop(tCircular_buffer *hBuffer, uint8_t *data);
eCircular_buffer_status tCircular_buffer_clear(tCircular_buffer *hBuffer);

#endif

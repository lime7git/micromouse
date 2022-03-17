#include "circular_buffer.h"

eCircular_buffer_status tCircular_buffer_init(tCircular_buffer *hBuffer, uint16_t size)
{
		hBuffer->buffer = malloc(size);
    hBuffer->head = 0;
		hBuffer->tail = 0;
		hBuffer->size = size;
		hBuffer->commands_cnt = 0;
		return BUFFER_SUCCESS;
}

eCircular_buffer_status tCircular_buffer_push(tCircular_buffer *hBuffer, uint8_t data)
{
		int next_head;
    next_head = (hBuffer->head + 1) % hBuffer->size;  // next is where head will point to after this write.
	
    if (next_head == hBuffer->tail)  // if the head + 1 == tail, circular buffer is full
        return BUFFER_FULL;
		
    hBuffer->buffer[hBuffer->head] = data;  // Load data and then move
		if(hBuffer->buffer[hBuffer->head] == '#') hBuffer->commands_cnt++;
		
    hBuffer->head = next_head;             // head to next data offset.

    return BUFFER_SUCCESS;  // return success to indicate successful push.
}

eCircular_buffer_status tCircular_buffer_pop(tCircular_buffer *hBuffer, uint8_t *data)
{
    if (hBuffer->head == hBuffer->tail)  // if the head == tail, we don't have any data
        return BUFFER_EMPTY;
		
		int next_tail;
		next_tail = (hBuffer->tail + 1) % hBuffer->size;  // next is where head will point to after this read.

    *data = hBuffer->buffer[hBuffer->tail];  // Read data and then move
		
		if(hBuffer->buffer[hBuffer->tail] == '#') hBuffer->commands_cnt--;
		
    hBuffer->tail = next_tail;              // tail to next offset.
    return BUFFER_SUCCESS;  // return success to indicate successful push.
}

eCircular_buffer_status tCircular_buffer_clear(tCircular_buffer *hBuffer)
{
	
	free(hBuffer->buffer);
	hBuffer->head = 0;
	hBuffer->tail = 0;
	hBuffer->commands_cnt = 0;
	
	return BUFFER_SUCCESS;
}

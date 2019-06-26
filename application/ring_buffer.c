/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include "ring_buffer.h"


bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize) 
{
	assert(ringBuffer);
	assert(dataBuffer);
	assert(dataBufferSize > 0);
	
	if ((ringBuffer==NULL) ||(dataBuffer==NULL)) {
	  return false;
	}
	
	ringBuffer->buffer = dataBuffer;
	ringBuffer->sizeOfBuffer = dataBufferSize/sizeof(char);
	ringBuffer->tail = -1;
	ringBuffer->head = 0;
	
	return true;
}

bool RingBuffer_Clear(RingBuffer *ringBuffer)
{
	assert(ringBuffer);
	
	if (ringBuffer==NULL) {
		return false;
	}
	
	ringBuffer->head = 0;
	ringBuffer->tail = -1;
	
	return true;
}

bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer)
{
    assert(ringBuffer);	
	
	if(ringBuffer==NULL){
	    return true;
	}
	
	return (ringBuffer->tail == -1);
}

size_t RingBuffer_GetLen(const RingBuffer *ringBuffer)
{
	assert(ringBuffer);
	
	if (ringBuffer==NULL || RingBuffer_IsEmpty(ringBuffer)) {
		return 0;
	}
	
	if(ringBuffer->tail < ringBuffer->head){
	    return ringBuffer->head - ringBuffer->tail;
	}
	else{
	    return ringBuffer->head + (ringBuffer->sizeOfBuffer - ringBuffer->tail);
	}
	
}

size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer)
{
	assert(ringBuffer);
	
	if (ringBuffer==NULL) {
		return 0;
	}
	
	return ringBuffer->sizeOfBuffer;
}


bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c)
{
	assert(ringBuffer);
	
	if (ringBuffer==NULL || ringBuffer->head == ringBuffer->tail) {
		return false;
	}
	
	ringBuffer->buffer[ringBuffer->head] = c;
	ringBuffer->head = (ringBuffer->head+1)%ringBuffer->sizeOfBuffer;
	
	if(ringBuffer->tail == -1){
	    ringBuffer->tail++;
	}
	
	return true;
}

bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c)
{
	assert(ringBuffer);
	assert(c);
	
    if ((ringBuffer==NULL) || (c==NULL) || RingBuffer_IsEmpty(ringBuffer)) {
		return false;
	}
	
	*c = ringBuffer->buffer[ringBuffer->tail];
	
	if((ringBuffer->tail + 1)%ringBuffer->sizeOfBuffer == ringBuffer->head){
	    RingBuffer_Clear(ringBuffer);
	}else{
	    ringBuffer->tail = (ringBuffer->tail + 1)%ringBuffer->sizeOfBuffer;
	}
	
	return true;
}


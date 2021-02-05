#ifndef __USER_RINGBUFFER_H
#define __USER_RINGBUFFER_H

#include <stm32f10x.h>

struct ringbuffer {
	uint8_t *data;
	uint32_t size;
	uint32_t readPos;
	uint32_t writePos;
};

struct ringbuffer *ringbuffer_create(uint32_t size);
void ringbuffer_destroy(struct ringbuffer *ringbuf);
uint32_t ringbuffer_put(struct ringbuffer *ringbuf,
                const uint8_t *dataIn, uint32_t dataLen);
uint32_t ringbuffer_get(struct ringbuffer *ring_buf,
		uint8_t *dataOut, uint32_t dataLen);
void ringbuffer_reset(struct ringbuffer *ringbuf);
uint8_t ringbuffer_is_empty(struct ringbuffer *ringbuf);
uint8_t ringbuffer_is_full(struct ringbuffer *ringbuf);

#endif

#include <stdlib.h>
#include <string.h>

#include "ringbuffer.h"

#define min(a,b)	(((a) < (b)) ? (a) : (b))

static __inline uint8_t is_power_of_2(uint32_t n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

static uint32_t roundup_power_of_2(uint32_t val)
{
	val--;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	val++;

	return val;
}

//bytes used
static __inline uint32_t ringbuffer_used(const struct ringbuffer *ringbuf)
{
	return ringbuf->writePos - ringbuf->readPos;
}

//bytes available
static __inline uint32_t ringbuffer_avail(const struct ringbuffer *ringbuf)
{
	return ringbuf->size - ringbuffer_used(ringbuf);
}

struct ringbuffer* ringbuffer_create(uint32_t size)
{
	struct ringbuffer *buf;

	if (!is_power_of_2(size)) {
		size = roundup_power_of_2(size);
	}

	buf = malloc(sizeof(struct ringbuffer));
	if (!buf) {
		return NULL;
	}

	buf->data = malloc(size);
	if (!buf->data) {
		free(buf);
		return NULL;
	}

	buf->size = size;
	buf->readPos = 0;
	buf->writePos = 0;

	return buf;
}

void ringbuffer_destroy(struct ringbuffer *buf)
{
	if (!buf)
		return;

	if (buf->data) {
		free(buf->data);
		buf->data = NULL;
	}

	free(buf);
}

uint32_t ringbuffer_put(struct ringbuffer *ringbuf,
		const uint8_t *dataIn, uint32_t dataLen)
{
	uint32_t tmp;
	uint32_t writePos;

	dataLen = min(dataLen, ringbuffer_avail(ringbuf));

	//put the data starting from writePos to buffer end
	writePos = ringbuf->writePos & (ringbuf->size - 1);
	tmp = min(dataLen, ringbuf->size - writePos);

	memcpy(ringbuf->data + writePos, dataIn, tmp);

	//put the rest at the beginning of the buffer
	memcpy(ringbuf->data, dataIn + tmp, dataLen - tmp);

	ringbuf->writePos += dataLen;

	return dataLen;
}

uint32_t ringbuffer_get(struct ringbuffer *ringbuf,
		uint8_t *dataOut, uint32_t dataLen)
{
	uint32_t tmp;
	uint32_t readPos;

	dataLen = min(dataLen, ringbuffer_used(ringbuf));

	//get the data from readPos until the end of the buffer
	readPos = ringbuf->readPos & (ringbuf->size - 1);
	tmp = min(dataLen, ringbuf->size - readPos);

	memcpy(dataOut, ringbuf->data + readPos, tmp);

	//get the rest from the beginning of the buffer
	memcpy(dataOut + tmp, ringbuf->data, dataLen - tmp);

	ringbuf->readPos += dataLen;

	return dataLen;
}

void ringbuffer_reset(struct ringbuffer *ringbuf)
{
	ringbuf->writePos = 0;
	ringbuf->readPos = 0;
}

uint8_t ringbuffer_is_empty(struct ringbuffer *ringbuf)
{
	return ringbuf->readPos == ringbuf->writePos;
}

uint8_t ringbuffer_is_full(struct ringbuffer *ringbuf)
{
	return ringbuffer_used(ringbuf) == ringbuf->size;
}

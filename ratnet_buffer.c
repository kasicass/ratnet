#include "ratnet_buffer.h"
#include <stdlib.h>
#include <string.h>

#define	DEFAULT_DATA_SIZE		1024

// check .rb_data is big enough ?
static void
buf_check_size(struct RNET_buffer *buffer, int need_nbytes)
{
	int rest_size, new_size;

	rest_size = RNET_buf_restsize(buffer);
	if ( rest_size >= need_nbytes )
		return;

	// give a big size, ok ?
	new_size = RNET_buf_size(buffer) + DEFAULT_DATA_SIZE + need_nbytes;
	buffer->rb_data = realloc(buffer->rb_data, new_size);
	buffer->rb_size = new_size;
}

struct RNET_buffer *
RNET_buf_new(void)
{
	struct RNET_buffer * buffer;
	buffer = (struct RNET_buffer *) malloc(sizeof(struct RNET_buffer));
	buffer->rb_cur  = 0;
	buffer->rb_size = DEFAULT_DATA_SIZE;
	buffer->rb_data = (char *) malloc(DEFAULT_DATA_SIZE);
	return (buffer);
}

void
RNET_buf_free(struct RNET_buffer * buffer)
{
	if ( RNET_buf_data(buffer) != NULL )
		free(RNET_buf_data(buffer));
	free(buffer);
}

// ------- push func

void
RNET_buf_push_int8(struct RNET_buffer *buffer, unsigned char val)
{
	buf_check_size(buffer, 1);
	*((unsigned char *) &(buffer->rb_data[buffer->rb_cur++])) = val;
}

void
RNET_buf_push_int16(struct RNET_buffer *buffer, unsigned short val)
{
	buf_check_size(buffer, 2);
	*((unsigned short *) &(buffer->rb_data[buffer->rb_cur])) = val;
	buffer->rb_cur += 2;
}

void
RNET_buf_push_int32(struct RNET_buffer *buffer, unsigned int val)
{
	buf_check_size(buffer, 4);
	*((unsigned int *) &(buffer->rb_data[buffer->rb_cur])) = val;
	buffer->rb_cur += 4;
}

void
RNET_buf_push_rawdata(struct RNET_buffer *buffer, const char *rawdata, int len)
{
	buf_check_size(buffer, len);
	memcpy(&buffer->rb_data[buffer->rb_cur], rawdata, len);
	buffer->rb_cur += len;
}

void
RNET_buf_push_string(struct RNET_buffer *buffer, const char *str)
{
	int len = (int) strlen(str);
	RNET_buf_push_int16(buffer, len);
	RNET_buf_push_rawdata(buffer, str, len);
}

// --------- pop func

unsigned char
RNET_buf_pop_int8(struct RNET_buffer *buffer)
{
	unsigned char val;
	val = *((unsigned char *) &(buffer->rb_data[buffer->rb_cur++]));
	return val;
}

unsigned short
RNET_buf_pop_int16(struct RNET_buffer *buffer)
{
	unsigned short val;
	val = *((unsigned short *) &(buffer->rb_data[buffer->rb_cur]));
	buffer->rb_cur += 2;
	return val;
}

unsigned int
RNET_buf_pop_int32(struct RNET_buffer *buffer)
{
	unsigned int val;
	val = *((unsigned int *) &(buffer->rb_data[buffer->rb_cur]));
	buffer->rb_cur += 4;
	return val;
}

void
RNET_buf_pop_rawdata(struct RNET_buffer *buffer, char *outbuf, int outbuf_len)
{
	memcpy( outbuf, &(buffer->rb_data[buffer->rb_cur]), outbuf_len );
	buffer->rb_cur += outbuf_len;
}

char *
RNET_buf_pop_string(struct RNET_buffer *buffer)
{
	int len;
	char *outdata;

	len = RNET_buf_pop_int16(buffer);
	outdata = (char *)calloc(1, len+1);		// FIXME: memory leak

	RNET_buf_pop_rawdata(buffer, outdata, len);
	return outdata;
}


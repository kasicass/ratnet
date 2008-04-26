#ifndef RATNET_BUFFER_H
#define RATNET_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif


struct RNET_buffer
{
	int   rb_cur;
	int   rb_size;
	char *rb_data;
};

#define	RNET_buf_cur(x)		((x)->rb_cur)
#define	RNET_buf_len(x)		((x)->rb_cur)
#define	RNET_buf_size(x)	((x)->rb_size)
#define	RNET_buf_data(x)	((x)->rb_data)
#define	RNET_buf_restsize(x)	(RNET_buf_size(x) - RNET_buf_len(x))


// pointer to buffer - ok
// NULL              - fail
struct RNET_buffer * RNET_buf_new(void);

//
void RNET_buf_free(struct RNET_buffer * buffer);

//
void RNET_buf_push_int8(struct RNET_buffer *buffer, unsigned char val);
void RNET_buf_push_int16(struct RNET_buffer *buffer, unsigned short val);
void RNET_buf_push_int32(struct RNET_buffer *buffer, unsigned int val);
void RNET_buf_push_rawdata(struct RNET_buffer *buffer, const char *rawdata, int len);
void RNET_buf_push_string(struct RNET_buffer *buffer, const char *str);

unsigned char  RNET_buf_pop_int8(struct RNET_buffer *buffer);
unsigned short RNET_buf_pop_int16(struct RNET_buffer *buffer);
unsigned int   RNET_buf_pop_int32(struct RNET_buffer *buffer);
void RNET_buf_pop_rawdata(struct RNET_buffer *buffer, char *outbuf, int outbuf_len);
char * RNET_buf_pop_string(struct RNET_buffer *buffer);


#ifdef __cplusplus
}
#endif

#endif


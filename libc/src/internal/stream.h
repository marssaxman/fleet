#ifndef _LIBC_INTERNAL_STREAM_H
#define _LIBC_INTERNAL_STREAM_H

#define STREAM_READ 0x01 // currently reading from the stream
#define STREAM_WRITE 0x02 // currently writing to the stream
#define STREAM_EOF 0x04 // C standard feof() indicator
#define STREAM_ERR 0x08 // C standard ferror() indicator
#define STREAM_LINESYNC 0x0F // must flush buffer after each newline

struct _stream
{
	unsigned state;
	int id;
};

#endif //_LIBC_INTERNAL_STREAM_H

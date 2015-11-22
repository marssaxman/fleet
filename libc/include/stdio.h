#ifndef _STDIO_H
#define _STDIO_H

#include <_va_list.h>
#include <_null.h>
#include <_size_t.h>

#define EOF (-1)
#define BUFSIZ 1024

// setvbuf modes: no buffer, line buffer, or full buffer
#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

typedef struct _stream FILE;

FILE *stdin;
FILE *stdout;
FILE *stderr;

int fclose(FILE *stream);
int fflush(FILE *stream);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t bytes);

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int sprintf(char *str, const char *format, ...);
int snprintf(char *s, size_t max, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int vsnprintf(char *s, size_t max, const char *format, va_list arg);

int fgetc(FILE *stream);
char *fgets(char *str, int num, FILE *stream);
int getc(FILE *stream);
int getchar();
int fputc(int character, FILE *stream);
int fputs(const char *str, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int character);
int puts(const char *str);

size_t fread(void *dest, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *src, size_t size, size_t count, FILE *stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);

#if 0 // implement later
FILENAME_MAX
FOPEN_MAX
L_tmpnam
SEEK_CUR
SEEK_END
SEEK_SET
TMP_MAX
fgetpos
fopen
fpos_t
fprintf
freopen
fscanf
fseek
fsetpos
ftell
gets
perror
printf
remove
rename
rewind
scanf
sscanf
tmpfile
tmpnam
ungetc
vfscanf
vprintf
vscanf
vsscanf
#endif // to implement

#endif //_STDIO_H


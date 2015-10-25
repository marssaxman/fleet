#ifndef _STDIO_H
#define _STDIO_H

#include <_va_list.h>
#include <_null.h>
#include <_size_t.h>

#define EOF (-1)
#define BUFSIZ 1024

typedef struct _stream FILE;

FILE *stdin;
FILE *stdout;
FILE *stderr;

int fclose(FILE *stream);
int fflush(FILE *stream);

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
int sprintf(char *str, const char *format, ...);
int snprintf(char *s, size_t max, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int vsnprintf(char *s, size_t max, const char *format, va_list arg);
FILENAME_MAX
FOPEN_MAX
L_tmpnam
SEEK_CUR
SEEK_END
SEEK_SET
TMP_MAX
_IOFBF
_IOLBF
_IONBF
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
setbuf
setvbuf
sscanf
tmpfile
tmpnam
ungetc
vfprintf
vfscanf
vprintf
vscanf
vsscanf
#endif // to implement

#endif //_STDIO_H


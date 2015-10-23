#ifndef _STDIO_H
#define _STDIO_H

#include <_va_list.h>
#include <_null.h>
#include <_size_t.h>

int sprintf(char *str, const char *format, ...);
int snprintf(char *s, size_t max, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int vsnprintf(char *s, size_t max, const char *format, va_list arg);

#if 0 // implement later
BUFSIZ
EOF
FILE
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
clearerr
fclose
feof
ferror
fflush
fgetc
fgetpos
fgets
fopen
fpos_t
fprintf
fputc
fputs
fread
freopen
fscanf
fseek
fsetpos
ftell
fwrite
getc
getchar
gets
perror
printf
putc
putchar
puts
remove
rename
rewind
scanf
setbuf
setvbuf
sscanf
stderr
stdin
stdout
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


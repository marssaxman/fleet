// Copyright (C) 2015 Mars Saxman. All rights reserved.
// Permission is granted to use at your own risk and distribute this software
// in source and binary forms provided all source code distributions retain
// this paragraph and the above copyright notice. THIS SOFTWARE IS PROVIDED "AS
// IS" WITH NO EXPRESS OR IMPLIED WARRANTY.

#ifndef _STDIO_H
#define _STDIO_H

#include <c/va_list.h>
#include <c/null.h>
#include <c/size_t.h>

#define EOF (-1)
#define BUFSIZ 1024
#define FILENAME_MAX 4096
#define FOPEN_MAX 8
#define TMP_MAX 10000
#define L_tmpnam 20

// setvbuf modes: no buffer, line buffer, or full buffer
#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

typedef struct _stream FILE;

FILE *stdin;
FILE *stdout;
FILE *stderr;

FILE *fopen(const char *filename, const char *mode);
FILE *freopen(const char *filename, const char *mode, FILE *stream);
int fclose(FILE *stream);
int fflush(FILE *stream);
void setbuf(FILE *stream, char *buffer);
int setvbuf(FILE *stream, char *buffer, int mode, size_t bytes);
int remove(const char *filename);
int rename(const char *oldname, const char *newname);
FILE *tmpfile(void);
char *tmpnam(char *s);

size_t fread(void *dest, size_t size, size_t count, FILE *stream);
size_t fwrite(const void *src, size_t size, size_t count, FILE *stream);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
typedef long long int fpos_t;
int fgetpos(FILE *stream, fpos_t *pos);
int fseek(FILE *stream, long int offset, int origin);
int fsetpos(FILE *stream, const fpos_t *pos);
long int ftell(FILE *stream);
void rewind(FILE *stream);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int vfprintf(FILE *stream, const char *format, va_list arg);
int sprintf(char *str, const char *format, ...);
int snprintf(char *s, size_t max, const char *format, ...);
int vsprintf(char *s, const char *format, va_list arg);
int vsnprintf(char *s, size_t max, const char *format, va_list arg);
int scanf(const char *format, ...);
int vscanf(const char *format, va_list arg);
int fscanf(FILE *stream, const char *format, ...);
int vfscanf(FILE *stream, const char *format, va_list arg);
int sscanf(const char *buffer, const char *format, ...);
int vsscanf(const char *buffer, const char *format, va_list arg);

int fgetc(FILE *stream);
char *fgets(char *str, int num, FILE *stream);
int getc(FILE *stream);
int getchar();
int fputc(int character, FILE *stream);
int fputs(const char *str, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int character);
int puts(const char *str);
int ungetc(int character, FILE *stream);

#ifdef __GNUC__
char *gets(char *s) __attribute__((deprecated));
#else
char *gets(char *s);
#endif

#endif //_STDIO_H


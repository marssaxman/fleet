#ifndef _ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#define EDOM 1
#define ERANGE 2
#define EILSEQ 3

#ifdef __cplusplus
}
#endif

#endif //_ERRNO_H

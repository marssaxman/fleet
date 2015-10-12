#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define assert(cond) ((void)0)
#else
extern void _assert(const char *file, const char *line, const char *cond);
#define assert(cond) ((cond)? ((void)0): _assert(__FILE__, __LINE__, #cond))
#endif

#ifdef __cplusplus
}
#endif

#endif //_ASSERT_H

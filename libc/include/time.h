#ifndef _TIME_H
#define _TIME_H

#include <_null.h>
#include <_size_t.h>
#include <_struct_tm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int clock_t;
typedef int time_t;
// millisecond clock
#define CLOCKS_PER_SEC 1000

clock_t clock(void);
double difftime(time_t end, time_t start);
time_t mktime(struct tm*);
time_t time(time_t *out);
char *asctime(const struct tm*);
char *ctime(const time_t*);
struct tm *gmtime(const time_t*);
struct tm *localtime(const time_t*);
size_t strftime(char *buf, size_t size, const char *format, const struct tm*);

#ifdef __cplusplus
}
#endif

#endif //_TIME_H


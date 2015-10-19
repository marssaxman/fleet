#ifndef _TIME_H
#define _TIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int clock_t;
typedef int time_t;
struct tm
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};
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


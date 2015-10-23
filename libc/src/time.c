#include <time.h>

#if 0
clock_t clock(void)
{
}

time_t time(time_t *out)
{
}
#endif

double difftime(time_t end, time_t start)
{
	return (double)(end - start);
}

#if 0
time_t mktime(struct tm *t)
{
}

struct tm *gmtime(const time_t *t)
{
}

struct tm *localtime(const time_t *t)
{
}
#endif

char *asctime(const struct tm *t)
{
	// "Www Mmm dd hh:mm:ss yyyy\n"
	static char buf[26];
	strftime(buf, 26, "%a %b %d %H:%M:%S %Y\n", t);
	return buf;
}

char *ctime(const time_t *t)
{
	return asctime(localtime(t));
}

static char *fmt(char *buf, char *end, const char *format, const struct tm *t);

static char *out_ch(char *buf, char *end, char ch)
{
	if (buf >= end) return NULL;
	*buf++ = ch;
	return buf;
}

static char *out_str(char *buf, char *end, const char *str)
{
	while (*str) {
		if (buf >= end) return NULL;
		*buf++ = *str++;
	}
	return buf;
}

static char *out_dec(char *buf, char *end, int n, int val, char pad)
{
	if (buf + n >= end) return NULL;
	while (n--) {
		buf[n] = (val > 0)? (val % 10 + '0'): ' ';
		val /= 10;
	}
	return buf + n;
}

static char *abbreviated_weekday_name(char *buf, char *end, const struct tm *t)
{
	if (t->tm_wday < 0 || t->tm_wday > 6) return NULL;
	static char *names[] = {
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	return out_str(buf, end, names[t->tm_wday]);
}

static char *full_weekday_name(char *buf, char *end, const struct tm *t)
{
	if (t->tm_wday < 0 || t->tm_wday > 6) return NULL;
	static char *names[] = {
			"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
			"Saturday"
	};
	return out_str(buf, end, names[t->tm_wday]);
}

static char *abbreviated_month_name(char *buf, char *end, const struct tm *t)
{
	if (t->tm_mon < 0 || t->tm_mon > 11) return NULL;
	static char *names[] = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
			"Oct", "Nov", "Dec"
	};
	return out_str(buf, end, names[t->tm_mon]);
}

static char *full_month_name(char *buf, char *end, const struct tm *t)
{
	if (t->tm_mon < 0 || t->tm_mon > 11) return NULL;
	static char *names[] = {
			"January", "February", "March", "April", "May", "June", "July",
			"August", "September", "October", "November", "December"
	};
	return out_str(buf, end, names[t->tm_mon]);
}

static char *week_based_year(char *buf, char *end, int n, const struct tm *t)
{
	// TODO
	return NULL;
}

static char *out_hour_12H(char *buf, char *end, const struct tm *t)
{
	int hour = t->tm_hour % 12;
	return out_dec(buf, end, 2, hour? hour: 12, '0');
}

static char *out_fmt(char *buf, char *end, char format, const struct tm *t)
{
	switch (format) {
		case 'a': return abbreviated_weekday_name(buf, end, t);
		case 'A': return full_weekday_name(buf, end, t);
		case 'b': return abbreviated_month_name(buf, end, t);
		case 'B': return full_month_name(buf, end, t);
		case 'c': return fmt(buf, end, "%m/%d/%y %H:%M:%S", t);
		case 'C': return fmt(buf, end, "%a %b %e %H:%M:%S %Y", t);
		case 'd': return out_dec(buf, end, 2, t->tm_mday, '0');
		case 'D': return fmt(buf, end, "%m/%d/%y", t);
		case 'e': return out_dec(buf, end, 2, t->tm_mday, ' ');
		case 'F': return fmt(buf, end, "%Y-%m-%d", t);
		case 'g': return week_based_year(buf, end, 2, t);
		case 'G': return week_based_year(buf, end, 4, t);
		case 'h': return abbreviated_month_name(buf, end, t);
		case 'H': return out_dec(buf, end, 2, t->tm_hour, '0');
		case 'I': return out_hour_12H(buf, end, t);

// continue from here:
// http://www.cplusplus.com/reference/ctime/strftime/
// examples:
// http://www.opensource.apple.com/source/Libc/Libc-167/string.subproj/strftime.c
// http://mirror.fsf.org/pmon2000/3.x/src/sdk/libc/time/strftime.c

		case '%':
		default: return out_ch(buf, end, format);
	}
}

static char *fmt(char *buf, char *end, const char *format, const struct tm *t)
{
	for (; buf && *format; ++format) {
		char ch = *format;
		if ('%' == ch && format[1]) {
			buf = out_fmt(buf, end, *++format, t);
		} else {
			buf = out_ch(buf, end, ch);
		}
	}
	return buf;
}

size_t strftime(char *buf, size_t size, const char *format, const struct tm *t)
{
	char *end = &buf[size];
	char *dest = fmt(buf, end, format, t);
	dest = out_ch(dest, end, '\0');
	return dest? dest - buf: 0;
}


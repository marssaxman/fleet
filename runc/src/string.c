#include "string.h"
#include "errno.h"

char *strcpy(char *dest, const char *src)
{
	char *out = dest;
	while (*dest++ = *src++) {}
	return out;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (n--) {
		*dest++ = *src;
		if (*src) src++;
	}
	return out;
}

int strcmp(const char *l, const char *r)
{
	while (*l && *l == *r) {
		l++;
		r++;
	}
	return *(const unsigned char *)l - *(const unsigned char *)r;
}

int strncmp(const char *l, const char *r, size_t n)
{
	while (n--) {
		unsigned char lch = *(unsigned char*)l++;
		unsigned char rch = *(unsigned char*)r++;
		if (lch != rch) return lch - rch;
		if (!lch) break;
	}
	return 0;
}

int strcoll(const char *l, const char *r)
{
	return strcmp(l, r);
}

char *strcat(char *dest, const char *src)
{
	char *out = dest;
	while (*dest) dest++;
	while (*dest++ = *src++) {}
	return out;
}

char *strncat(char *dest, const char *src, size_t n)
{
	char *out = dest;
	while (*dest) dest++;
	while (n-- && (*dest++ = *src++)) {
		if (0 == n) {
			*dest = '\0';
		}
	}
	return out;
}

char *strchr(const char *str, int ch)
{
	while (*str && *str != ch) {
		str++;
	}
	return (*str == ch) ? (char*)str : NULL;
}

char *strrchr(const char *str, int ch)
{
	const char *out = NULL;
	do {
		if (*str == ch) {
			out = str;
		}
	} while (*str++);
	return (char*)out;
}

char *strstr(const char *haystack, const char *needle)
{
	if (!*needle) return (char*)haystack;
	if (!*haystack) return NULL;
	size_t n = strlen(needle);
	for (;haystack = strchr(haystack, *needle); haystack++) {
		if (!strncmp(haystack, needle, n)) {
			return (char*)haystack;
		}
	}
	return NULL;
}

void *memchr(const void *buf, int ch, size_t n)
{
	const unsigned char *src = buf;
	while (n--) {
		if (*src == (unsigned char)ch) {
			return (void*)src;
		}
		src++;
	}
	return NULL;
}

size_t strspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c == '\0') break;
		str++;
	}
	return str - start;
}

size_t strcspn(const char *str, const char *set)
{
	const char *start = str;
	while (*str) {
		const char *c;
		for (c = set; *c; c++) {
			if (*str == *c) break;
		}
		if (*c != '\0') break;
		str++;
	}
	return str - start;
}

char *strpbrk(const char *str, const char *set)
{
	while (*str) {
		for (const char *c = set; *c; c++) {
			if (*str == *c) return (char*)str;
		}
		str++;
	}
	return NULL;
}

char *strtok_r(char *str, const char *delims, char **lasts)
{
	char *sbegin, *send;
	sbegin = str ? str : *lasts;
	sbegin += strspn(sbegin, delims);
	if (*sbegin == '\0') {
		*lasts = "";
		return NULL;
	}
	send = sbegin + strcspn(sbegin, delims);
	if (*send != '\0') {
		*send++ = '\0';
	}
	*lasts = send;
	return sbegin;
}

char *strtok(char *str, const char *delims)
{
	static char *save = "";
	return strtok_r(str, delims, &save);
}

size_t strlen(const char *str)
{
	const char *p = str;
	while (*p != '\0') {
		p++;
	}
	return (size_t)(p - str);
}

size_t strxfrm(char *dest, const char *src, size_t n)
{
	size_t out = 0;
	while (n--) {
		if (*dest++ = *src++) {
			++out;
		} else {
			return out;
		}
	}
	while (*src) {
		++src;
		++out;
	}
	return out;
}


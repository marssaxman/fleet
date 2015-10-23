#include <string.h>

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

#ifdef TESTSUITE
TESTSUITE(memchr) {
	static const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	CHECK(&lower[0] == memchr(lower, 'a', 26));
	CHECK(&lower[3] == memchr(lower, 'd', 26));
	char buf[] = "abcabcabc";
	CHECK(&buf[1] == memchr(buf, 'b', 8));
	CHECK(&lower[25] == memchr(lower, 'z', 26));
	CHECK(0 == memchr(lower, 'A', 26));
	CHECK(&lower[26] == memchr(lower, '\0', 27));
	CHECK(0 == memchr("", 'X', 0));
	char dummy[] = "";
	CHECK(dummy == memchr(dummy, '\0', 1));
	static const char words[] = "The quick brown fox jumped over the lazy "
			"dog's back.";
	void *ret = memchr(words, 'k', sizeof(words));
	CHECK(0 == strcmp(ret, &words[8]));
	CHECK(0 == memchr(words, 'Q', sizeof(words)));
	CHECK(&words[sizeof(words)-1] == memchr(words, '\0', sizeof(words)));
}
#endif



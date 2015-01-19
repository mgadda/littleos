#ifndef _STRING_H_
#define _STRING_H_

void *memcpy(void *restrict dst, const void *restrict src, unsigned int n);
char *itoa(int val, char *buf, int radix);
unsigned int strlen(const char *buf);
#endif /* _STRING_H_ */
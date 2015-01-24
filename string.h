#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

char *itoa(int val, char *buf, int radix);
unsigned int strlen(const char *buf);
void *memset(void *s, int c, size_t n);
#endif /* _STRING_H_ */
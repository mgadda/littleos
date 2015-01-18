#include "string.h"

char *itoa(int val, char *buf, int radix) {
  unsigned int i = 0;
  unsigned int start = i;
  if (val < 0 && radix == 10) {
    buf[i++] = '-';
    start = i;
  }

  if (radix == 16) {
    buf[i++] = '0';
    buf[i++] = 'x';
    start = i;
  }

  int x = val;
  do {
    int a = x % radix;
    if (a < 10) buf[i++] = a + '0';
    else buf[i++] = a + 'a' - 10;
  } while (x /= radix);

  char *s = buf+start;
  char *e = s+(i-3);

  while(s < e) {
    char t = *s;
    *s = *e;
    *e = t;
    s++; e--;
  }

  buf[i] = 0;
  return buf;
}

unsigned int strlen(const char *buf) {
  unsigned int i=0;
  while(buf[i] != 0) i++;
  return i;
}
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "string.h"
#include "framebuffer.h"

char *itoa(int val, char *buf, int radix) {
  uint32_t i = 0;
  uint32_t start = i;
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
  char *e = buf+(i-1);

  while(s < e) {
    char t = *s;
    *s = *e;
    *e = t;
    s++; e--;
  }

  buf[i] = 0;
  return buf;
}

char *uitoa(uint32_t val, char *buf, int radix) {
  uint32_t i = 0;
  uint32_t start = i;

  if (radix == 16) {
    buf[i++] = '0';
    buf[i++] = 'x';
    start = i;
  }

  uint32_t x = val;
  do {
    uint32_t a = x % radix;
    if (a < 10) buf[i++] = a + '0';
    else buf[i++] = a + 'a' - 10;
  } while (x /= radix);

  char *s = buf+start;
  char *e = buf+(i-1);

  while(s < e) {
    char t = *s;
    *s = *e;
    *e = t;
    s++; e--;
  }

  buf[i] = 0;
  return buf;
}

size_t strlen(const char *buf) {
  unsigned int i=0;
  while(buf[i] != 0) i++;
  return i;
}

// Terribly naive implementation of memset to get things compiling
// See http://www.xs-labs.com/en/blog/2013/08/06/optimising-memset/
// for more details
void *memset(void *s, int c, size_t n) {
  char *mem = (char*)s;
  size_t i;
  for (i=0; i<n; i++) {
    mem[i] = (uint8_t)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t len) {
  char *dstmem = (char*)dst;
  char *srcmem = (char*)src;
  size_t i;
  for (i=0; i<len; i++) {
    dstmem[i] = srcmem[i];
  }
  return dstmem;
}

#ifdef DEBUG
static uint32_t line_count = 0;
#endif

int printf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);

#ifdef DEBUG
  char line_count_str[20];
  fb_write_str(uitoa(line_count++, line_count_str, 10));
  fb_write_str(": ");
#endif

  size_t i;
  char buf[20];
  int val;
  int32_t uval;
  char c, *s;
  for (i=0; i<strlen(format); i++) {
    if (format[i] == '%') {
      i++;
      while (format[i] == ' ') i++;

      switch(format[i]) {
        case 'i':
          val = va_arg(ap, int);
          itoa(val, buf, 10);
          fb_write_str(buf);
          break;
        case 'x':
          uval = va_arg(ap, uint32_t);
          uitoa(uval, buf, 16);
          fb_write_str(buf);
          break;
        case 'c':
          c = (char)va_arg(ap, int);
          fb_write(&c, 1);
          break;
        case 's':
          s = va_arg(ap, char*);
          fb_write_str(s);
        default:
          fb_write((char*)format+i, 1);
      }
    } else {
      fb_write((char*)format+i, 1);
    }
  }

  va_end(ap);
  return 0;
}
#include "serial.h"
#include "framebuffer.h"
#include "string.h"

void kernel() {
  fb_clear();
  char greeting[] = "No. 5 is alive!";
  fb_write(greeting, strlen(greeting));
  serial_write(greeting, strlen(greeting));
  return;
}

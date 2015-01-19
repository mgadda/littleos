#include "serial.h"
#include "framebuffer.h"
#include "string.h"
#include "multiboot.h"

void kernel(multiboot_info_t *info) {
  fb_clear();
  char greeting[] = "No. 5 is alive!\n";
  fb_write(greeting, strlen(greeting));
  serial_write(greeting, strlen(greeting));

  char flags[100];
  itoa(info->flags, flags, 16);
  fb_write(flags, strlen(flags));
  return;
}

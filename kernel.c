#include "serial.h"
#include "framebuffer.h"
#include "string.h"
#include "multiboot.h"
#include "descriptor_tables.h"

void kernel(multiboot_info_t *info) {
  init_descriptor_tables();

  fb_clear();
  char greeting[] = "No. 5 is alive!\n";
  fb_write(greeting, strlen(greeting));
  serial_write(greeting, strlen(greeting));

  char flags[100];
  itoa(info->flags, flags, 16);
  fb_write(flags, strlen(flags));
  return;
}

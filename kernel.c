#include "serial.h"
#include "framebuffer.h"
#include "string.h"
#include "multiboot.h"
#include "debug.h"
#include "descriptor_tables.h"

void kernel(multiboot_info_t *info) {
  init_descriptor_tables();

  fb_clear();
  char greeting[] = "No. 5 is alive!\n";
  fb_write_str(greeting);
  serial_write(greeting, strlen(greeting));

  char flags[100];
  itoa(info->flags, flags, 16);
  fb_write_str(flags);

  asm volatile ("int $0x3");
  asm volatile ("int $0x4");
  return;
}

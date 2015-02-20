#include "log.h"
#include "descriptor_tables.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "serial.h"
#include "string.h"
#include "timer.h"
#include "paging.h"

void kernel(multiboot_info_t *info) {
  fb_clear();
  info("No. 5 is alive!");

  char greeting[] = "No. 5 is alive!";
  serial_write(greeting, strlen(greeting));

  debug("multiboot header flags: %x", info->flags);

  init_descriptor_tables();

  debug("Generating random interrupts...");
  asm volatile ("int $0x3");
  asm volatile ("int $0x4");

  //init_timer(19);
  init_keyboard();
  init_paging();
  return;
}

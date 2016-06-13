#include <stdbool.h>

#include "log.h"
#include "descriptor_tables.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "serial.h"
#include "string.h"
#include "timer.h"
#include "paging.h"
#include "isr.h"

bool int3_happened = false;
bool int4_happened = false;

void int3_handler(registers_t regs) {
  debug("interrupt 3 detected.");
  int3_happened = true;
}

void int4_handler(registers_t regs) {
  debug("interrupt 4 detected.");
  int4_happened = true;
}

void kernel_main(multiboot_info_t *info) {
  fb_clear();
  info("No. 5 is alive!");

  char greeting[] = "No. 5 is alive!";
  serial_write(greeting, strlen(greeting));

  debug("multiboot header flags: %x", info->flags);

  init_descriptor_tables();

  debug("Generating random interrupts...");
  register_interrupt_handler(3, int3_handler);
  register_interrupt_handler(4, int4_handler);
  asm volatile ("int $0x3");
  asm volatile ("int $0x4");
  if (!int3_happened) PANIC("interrupts improperly configured, no point in continuing.");
  if (!int4_happened) PANIC("interrupts improperly configured, no point in continuing.");

  //init_timer(19);
  init_keyboard();
  //init_paging();

  // attempt to access address from unmapped page of memory:
  int32_t *fault = (int32_t*)0xC0100004;
  int32_t value = *fault;
  return;
}

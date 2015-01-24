// from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#include <stdint.h>
#include "isr.h"
#include "string.h"
#include "framebuffer.h"
#include "isr.h"

void isr_handler(registers_t regs) {
  fb_write_str("\nreceived interrupt: ");
  char buf[3]; // "0" - "255"
  fb_write_str(itoa(regs.int_no, buf, 10));
  fb_write_str("\n");
}
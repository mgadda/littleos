#include "io.h"
#include "serial.h"
#include "framebuffer.h"
#include "debug.h"

void kernel() {
  serial_write("No. 5 is alive!", 15);
  fb_write("No. 5 is alive!", 15);
  return;
}

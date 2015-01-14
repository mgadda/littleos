#include "io.h"
#include "serial.h"
#include "framebuffer.h"

void kmain() {
  serial_write("No. 5 is alive!", 15);
  fb_write("No. 5 is alive!", 15);
  return;
}

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "io.h"
#include "framebuffer.h"

/**
 * fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * within the frame buffer.
 * fb and bg must be between 0 and 15
 * i must be between 0 and 80*25 = 2000
 */
void fb_write_cell(short i, char c, unsigned char fg, unsigned char bg) {
  char *fb = (char*)0x00B8000;
  fb[i*2] = c;
  fb[i*2 + 1] = ((bg & 0x0f) << 4) | (fg & 0x0f);
}

void fb_move_cursor(unsigned short pos) {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}

void fb_write(char *buf, unsigned int len) {
  unsigned int pos = 0;
  unsigned int i;
  for (i=0; i<len; ++i) {
    fb_write_cell(pos++, buf[i], FB_WHITE, FB_BLACK);
  }
  fb_move_cursor(len);
  
}

#endif /* _FRAMEBUFFER_H_ */


#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "io.h"
#include "framebuffer.h"

#define FRAMEBUFFER_ADDR 0x00B8000;
#define FB_WIDTH 80
#define FB_HEIGHT 25

static unsigned int fb_pos_x = 0;
static unsigned int fb_pos_y = 0;

/**
 * fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * within the frame buffer.
 * fb and bg must be between 0 and 15
 * i must be between 0 and 80*25 = 2000
 */
void fb_write_cell(short i, char c, unsigned char fg, unsigned char bg) {
  unsigned char *fb = (unsigned char*)FRAMEBUFFER_ADDR;
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
  unsigned int i;
  unsigned short pos;
  for (i=0; i<len; i++) {
    char c = buf[i];
    if (c == '\n' || c == '\r') {
      fb_pos_y++;
      fb_pos_x = 0;
      pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
      fb_move_cursor(pos+1);
      continue;
    } else {
      pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
      fb_write_cell(pos, c, FB_WHITE, FB_BLACK);
      fb_move_cursor(pos+1);
    }

    // handle position at end of line
    if (fb_pos_x == FB_WIDTH) {
      fb_pos_y++;
      fb_pos_x = 0;
    } else {
      fb_pos_x++;
    }
  }
}

void fb_clear() {
  fb_pos_x = 0;
  fb_pos_y = 0;

  int i;
  for (i=0; i<FB_WIDTH*FB_HEIGHT; i++) {
    fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
  }
}
#endif /* _FRAMEBUFFER_H_ */


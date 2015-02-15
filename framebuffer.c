#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "framebuffer.h"
#include "string.h"
#include "serial.h"

#define FRAMEBUFFER_ADDR 0x00B8000;
#define FB_WIDTH 80
#define FB_HEIGHT 25
#define TAB "  "

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

// move cursor to next line
//
void fb_newline() {
  if (fb_pos_y >= FB_HEIGHT-1)
    fb_shift_up();
  else
    fb_pos_y++;

  fb_pos_x = 0;
  fb_move_cursor(fb_pos_x + (fb_pos_y * FB_WIDTH));
}

// advances cursor forward one character
// if at end of line, wrap to next line
void fb_advance_pos() {
  if (fb_pos_x >= FB_WIDTH-1)
    fb_newline();
  else
    fb_pos_x++;

  fb_move_cursor(fb_pos_x + (fb_pos_y * FB_WIDTH));
}

void fb_write(char *buf, unsigned int len) {
  unsigned int i;
  uint16_t pos;
  for (i=0; i<len; i++) {
    char c = buf[i];
    if (c == '\n' || c == '\r') {
      fb_newline();
    } else if (c == '\t') {
      fb_write_str(TAB);
    } else {
      pos = fb_pos_x + (fb_pos_y * FB_WIDTH);
      fb_write_cell(pos, c, FB_WHITE, FB_BLACK);
      fb_advance_pos();
    }
  }
}

void fb_write_str(char *buf) {
  fb_write(buf, strlen(buf));
}

void fb_clear() {
  fb_pos_x = 0;
  fb_pos_y = 0;

  int i;
  for (i=0; i<FB_WIDTH*FB_HEIGHT; i++) {
    fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
  }
  fb_move_cursor(0);
}

void fb_clear_row(uint8_t row) {
  size_t i;
  for (i=0; i<FB_WIDTH; i++) {
    fb_write_cell((row*FB_WIDTH)+i, ' ', FB_WHITE, FB_BLACK);
  }
}

void fb_shift_up() {
  // use 16-bits here because each cell is 16-bits wide
  // this makes the pointer arithmetic work out correctly here
  uint16_t *fb = (uint16_t*)FRAMEBUFFER_ADDR;
  memmove(fb, fb+FB_WIDTH, FB_WIDTH*2*(FB_HEIGHT*2-1));
  fb_clear_row(FB_HEIGHT-1);
}

void fb_wrap_vertical() {
  fb_pos_y = 0;
  fb_clear_row(fb_pos_y);
  fb_move_cursor(0);
}

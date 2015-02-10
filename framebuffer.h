#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <stdint.h>

#define FB_BLACK          0
#define FB_BLUE           1
#define FB_GREEN          2
#define FB_CYAN           3
#define FB_RED            4
#define FB_MAGENTA        5
#define FB_BROWN          6
#define FB_LIGHT_GREY     7
#define FB_DARK_GREY      8
#define FB_LIGHT_BLUE     9
#define FB_LIGHT_GREEN    10
#define FB_LIGHT_CYAN     11
#define FB_LIGHT_RED      12
#define FB_LIGHT_MAGENTA  13
#define FB_LIGHT_BROWN    14
#define FB_WHITE    15

/* The Framebuffer I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The Framebuffer I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/**
 * fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * within the frame buffer.
 * fb and bg must be between 0 and 15
 * i must be between 0 and 80*25 = 2000
 */
void fb_write_cell(short i, char c, unsigned char fg, unsigned char bg);

/**
 * fb_move_cursor
 * Moves cursor forwarded int he frame buffer by pos characters
 */
void fb_move_cursor(unsigned short pos);

/**
 * fb_write
 * Writes c-string to framebuffer and advances cursor len number of characters
 * if len is not passed, string must be null terminated
 */
void fb_write(char *buf, unsigned int len);
void fb_write_str(char *buf);


/**
 * fb_clear
 * clear the screen
 */
void fb_clear();
void fb_clear_row(uint8_t row);
void fb_shift_up();
void fb_wrap_vertical();
void fb_newline();

#endif

// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

#ifndef __DESCRIPTOR_TABLES_H__
#define __DESCRIPTOR_TABLES_H__

#include <inttypes.h>

struct gdt_access {
  uint8_t type:4;           // 36 - 39: segment type - code segment
  uint8_t dt:1;             // 40: descriptor type
  uint8_t dpl:2;            // 41 - 42: descriptor privilege level - Ring 0 - 3
  uint8_t p:1;              // 43: is segment present? (1=yes)
} __attribute((packed));
typedef struct gdt_access gdt_access_t;

struct gdt_entry {
  uint16_t limit_low:16;    // 0 - 15
  uint32_t base_low:24;     // 16 - 35
  gdt_access_t access;      // 36 - 43
  uint8_t limit_high:4;     // 44 - 46
  uint8_t a:1;              // 47: available for system use, must be 0
  uint8_t unused:1;         // 48: must be 0
  uint8_t d:1;              // 49: operand size (0 = 16 bit, 1 = 32 bit)
  uint8_t g:1;              // 50: granularity (0 = 1 Byte, 1 = 1KiB)
  uint8_t base_high:8;      // 51 - 58
} __attribute((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr {
  uint16_t limit;
  gdt_entry_t *base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;

void init_descriptor_tables();


#endif
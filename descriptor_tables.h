// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

#ifndef __DESCRIPTOR_TABLES_H__
#define __DESCRIPTOR_TABLES_H__

#include <inttypes.h>

// ------------------------
// Global Descriptor Tables
// ------------------------

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

// ---------------------------
// Interrupt Descriptor Tables
// ---------------------------

struct idt_flags {
  uint8_t gate_type:3;  // 0 - 2, task, interrupt,
  uint8_t d:1;          // 3, gate size
  uint8_t zero:1;       // 4, value=0
  uint8_t dpl:2;        // 5 - 6, descriptor privilege level
  uint8_t p:1;          // 7, segment present
} __attribute__((packed));
typedef struct idt_flags idt_flags_t;

struct idt_entry {
  uint16_t base_low; // aka offset
  uint16_t segment_selector;
  uint8_t reserved; // always zero
  idt_flags_t flags;
  uint16_t base_high; // aka offset
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
  uint16_t limit;
  idt_entry_t *base; // aka offset
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
#endif
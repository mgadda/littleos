// based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

#include "descriptor_tables.h"

void gdt_flush(uint32_t);

// Internal use only
static void init_gdt();
static void gdt_set_gate();

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
// idt_entry_t idt_entries[256];
// idt_ptr_t   idt_ptr;

void init_descriptor_tables() {
  init_gdt();
}

static void init_gdt() {
  gdt_ptr.limit = sizeof(gdt_entry_t) * 5 - 1;
  gdt_ptr.base = gdt_entries;
  gdt_set_gate(0, 0, 0xffffffff,
    {.type: 10, .dt: 1, .dpl: 0, .p: 1},
    {.segment_length: 15, .a: 0, .unused: 0, .d: 1, .g: 1});
  gdt_set_gate(0, 0, 0xffffffff,
    {.type: 2, .dt: 1, .dpl: 0, .p: 1},
    {.segment_length: 15, .a: 0, .unused: 0, .d: 1, .g: 1});
  gdt_set_gate(0, 0, 0xffffffff,
    {.type: 10, .dt: 1, .dpl: 3, .p: 1},
    {.segment_length: 15, .a: 0, .unused: 0, .d: 1, .g: 1});
  gdt_set_gate(0, 0, 0xffffffff,
    {.type: 2, .dt: 1, .dpl: 3, .p: 1},
    {.segment_length: 15, .a: 0, .unused: 0, .d: 1, .g: 1});
}

static void gdt_set_gate(
  int32_t idx,
  uint32_t base,
  uint32_t limit,
  gdt_access_t access,
  gdt_granularity_t granularity) {

  gdt_entries[idx] = {
    .limit_low: limit & 0xffff,
    .base_low: limit & 0xffffff,
    .access: access,
    .granularity: granularity,
    .base_high: (base >> 24) & 0xffff
  }
}
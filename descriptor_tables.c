// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html

#include "descriptor_tables.h"

// Internal use only
extern void gdt_flush(gdt_ptr_t*);
static void init_gdt();
static void gdt_set_gate(
  int32_t idx,
  uint32_t base,
  uint32_t limit,
  gdt_access_t access,
  uint8_t d,
  uint8_t g);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
// idt_entry_t idt_entries[256];
// idt_ptr_t   idt_ptr;

void init_descriptor_tables() {
  init_gdt();
}

#define GDT_ACCESS_TYPE_CODE 10

// Granularity Field (1 bit)
#define GDT_GRANULARITY_BYTE 0
#define GDT_GRANULARITY_4KILOBYTE 1

// D Field (1 bit) - defines operand size for code segments
#define GDT_DEFAULT_OPERAND_SIZE_16 0
#define GDT_DEFAULT_OPERAND_SIZE_32 1

// DPL Field (2 bits)
#define GDT_DPL_RING0 0
#define GDT_DPL_RING1 1
#define GDT_DPL_RING2 2
#define GDT_DPL_RING3 3

// Segment Type Field (4 bits)
#define GDT_SEGMENT_TYPE_DATA 0
#define GDT_SEGMENT_TYPE_CODE 0x8

// Data Type
// E - expand direction
#define GDT_SEGMENT_TYPE_EXPAND_UP 0
#define GDT_SEGMENT_TYPE_EXPAND_DOWN 0x4

// W - read/write
#define GDT_SEGMENT_TYPE_READ_ONLY 0
#define GDT_SEGMENT_TYPE_READ_WRITE 0x2

// Code Type
// C - non/confirming
#define GDT_SEGMENT_TYPE_NONCONFORMING 0
#define GDT_SEGMENT_TYPE_CONFORMING 0x4

// R - execute/read
#define GDT_SEGMENT_TYPE_EXECUTE_ONLY 0
#define GDT_SEGMENT_TYPE_EXECUTE_READ 0x2

// A - accessed
#define GDT_SEGMENT_TYPE_RESET_ACCESSED 0

// Segment Present (p) - segment present in memory?
#define GDT_SEGMENT_NOT_PRESENT 0
#define GDT_SEGMENT_PRESENT 1

// Descriptor Type (S)
#define GDT_SEGMENT_DT_SYSTEM 0
#define GDT_SEGMENT_DT_CODE 1

static void init_gdt() {
  gdt_ptr.limit = sizeof(gdt_entry_t) * 5 - 1;
  gdt_ptr.base = gdt_entries;

  gdt_set_gate(0, 0, 0, (struct gdt_access){0, 0, 0, 0}, 0, 0);
  gdt_set_gate(
    1, // index
    0, 0xffffffff,
    (struct gdt_access){
      .type = GDT_SEGMENT_TYPE_CODE | GDT_SEGMENT_TYPE_EXECUTE_READ,
      .dt = GDT_SEGMENT_DT_CODE,
      .dpl = GDT_DPL_RING0,
      .p = GDT_SEGMENT_PRESENT
    }, // access
    GDT_DEFAULT_OPERAND_SIZE_32, // d
    GDT_GRANULARITY_4KILOBYTE // g
  );

  gdt_set_gate(
    2, // index
    0, 0xffffffff,
    (struct gdt_access){
      .type = GDT_SEGMENT_TYPE_DATA | GDT_SEGMENT_TYPE_READ_WRITE,
      .dt = GDT_SEGMENT_DT_CODE,
      .dpl = GDT_DPL_RING0,
      .p = GDT_SEGMENT_PRESENT
    }, // access
    GDT_DEFAULT_OPERAND_SIZE_32, // d
    GDT_GRANULARITY_4KILOBYTE // g
  );

  gdt_set_gate(
    3, // index
    0, 0xffffffff,
    (struct gdt_access){
      .type = GDT_SEGMENT_TYPE_CODE | GDT_SEGMENT_TYPE_EXECUTE_READ,
      .dt = GDT_SEGMENT_DT_CODE,
      .dpl = GDT_DPL_RING3,
      .p = GDT_SEGMENT_PRESENT
    }, // access
    GDT_DEFAULT_OPERAND_SIZE_32, // d
    GDT_GRANULARITY_4KILOBYTE // g
  );

  gdt_set_gate(
    4, // index
    0, 0xffffffff,
    (struct gdt_access){
      .type = GDT_SEGMENT_TYPE_DATA | GDT_SEGMENT_TYPE_READ_WRITE,
      .dt = GDT_SEGMENT_DT_CODE,
      .dpl = GDT_DPL_RING3,
      .p = GDT_SEGMENT_PRESENT
    }, // access
    GDT_DEFAULT_OPERAND_SIZE_32, // d
    GDT_GRANULARITY_4KILOBYTE // g
  );

  gdt_flush(&gdt_ptr);
}

static void gdt_set_gate(
  int32_t idx,
  uint32_t base,
  uint32_t limit,
  gdt_access_t access,
  uint8_t d,
  uint8_t g) {

  gdt_entries[idx] = (struct gdt_entry){
    .limit_low = limit & 0xffff,
    .base_low = base & 0xffffff,
    .access = access,
    .limit_high = (limit >> 16) & 0xf,
    .a = 0,
    .unused = 0,
    .d = d,
    .g = g,
    .base_high = (base >> 24) & 0xffff
  };
}

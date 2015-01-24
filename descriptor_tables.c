// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#include "string.h"
#include "descriptor_tables.h"

// Internal use only
extern void gdt_flush(gdt_ptr_t*);
extern void idt_flush(idt_ptr_t*);

static void init_gdt();
static void init_idt();

static void gdt_set_gate(
  int32_t idx,
  uint32_t base,
  uint32_t limit,
  gdt_access_t access,
  uint8_t d,
  uint8_t g);

static void idt_set_gate(
  uint8_t idx,
  void(*base),
  uint16_t selector,
  idt_flags_t flags);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void init_descriptor_tables() {
  init_gdt();
  init_idt();
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

// TODO: what is this?
#define IDT_SELECTOR 0x08

//Task type (3 bits)
#define IDT_GATE_TYPE_TASK 0x5
#define IDT_GATE_TYPE_INTERRUPT 0x6
#define IDT_GATE_TYPE_TRAP 0x7

// D (1 bit)
#define IDT_GATE_16BIT 0
#define IDT_GATE_32BIT 1

// Present (1 bit)
#define IDT_SEGMENT_NOT_PRESENT 0
#define IDT_SEGMENT_PRESENT 1

// DPL Field (2 bits)
#define IDT_DPL_RING0 0
#define IDT_DPL_RING1 1
#define IDT_DPL_RING2 2
#define IDT_DPL_RING3 3


static void init_idt() {
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = idt_entries;

  memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

  idt_flags_t flags = {
    .p = IDT_SEGMENT_PRESENT,
    .dpl = IDT_DPL_RING0,
    .zero = 0,
    .d = IDT_GATE_32BIT,
    .gate_type = IDT_GATE_TYPE_INTERRUPT
  };

  idt_set_gate(0, isr0, IDT_SELECTOR, flags);
  idt_set_gate(1, isr1, IDT_SELECTOR, flags);
  idt_set_gate(2, isr2, IDT_SELECTOR, flags);
  idt_set_gate(3, isr3, IDT_SELECTOR, flags);
  idt_set_gate(4, isr4, IDT_SELECTOR, flags);
  idt_set_gate(5, isr5, IDT_SELECTOR, flags);
  idt_set_gate(6, isr6, IDT_SELECTOR, flags);
  idt_set_gate(7, isr7, IDT_SELECTOR, flags);
  idt_set_gate(8, isr8, IDT_SELECTOR, flags);
  idt_set_gate(9, isr9, IDT_SELECTOR, flags);
  idt_set_gate(10, isr10, IDT_SELECTOR, flags);
  idt_set_gate(11, isr11, IDT_SELECTOR, flags);
  idt_set_gate(12, isr12, IDT_SELECTOR, flags);
  idt_set_gate(13, isr13, IDT_SELECTOR, flags);
  idt_set_gate(14, isr14, IDT_SELECTOR, flags);
  idt_set_gate(15, isr15, IDT_SELECTOR, flags);
  idt_set_gate(16, isr16, IDT_SELECTOR, flags);
  idt_set_gate(17, isr17, IDT_SELECTOR, flags);
  idt_set_gate(18, isr18, IDT_SELECTOR, flags);
  idt_set_gate(19, isr19, IDT_SELECTOR, flags);
  idt_set_gate(20, isr20, IDT_SELECTOR, flags);
  idt_set_gate(21, isr21, IDT_SELECTOR, flags);
  idt_set_gate(22, isr22, IDT_SELECTOR, flags);
  idt_set_gate(23, isr23, IDT_SELECTOR, flags);
  idt_set_gate(24, isr24, IDT_SELECTOR, flags);
  idt_set_gate(25, isr25, IDT_SELECTOR, flags);
  idt_set_gate(26, isr26, IDT_SELECTOR, flags);
  idt_set_gate(27, isr27, IDT_SELECTOR, flags);
  idt_set_gate(28, isr28, IDT_SELECTOR, flags);
  idt_set_gate(29, isr29, IDT_SELECTOR, flags);
  idt_set_gate(30, isr30, IDT_SELECTOR, flags);
  idt_set_gate(31, isr31, IDT_SELECTOR, flags);

  idt_flush(&idt_ptr);
}

static void idt_set_gate(
  uint8_t idx,
  void(*base),
  uint16_t selector,
  idt_flags_t flags) {

  idt_entries[idx] = (struct idt_entry){
    .base_low = (uint32_t)base & 0xffff,
    .base_high = ((uint32_t)base >> 16) & 0xffff,
    .segment_selector = selector,
    .flags = flags
  };
}

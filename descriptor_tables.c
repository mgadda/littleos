// (very loosely) based on code from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
// and also: http://www.jamesmolloy.co.uk/tutorial_html/5.-IRQs%20and%20the%20PIT.html
#include <stdbool.h>
#include <cpuid.h>

#include "string.h"
#include "descriptor_tables.h"
#include "io.h"
#include "apic.h"
#include "log.h"

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

// PIC - from http://wiki.osdev.org/8259_PIC
#define PIC1            0x20    /* IO base address for master PIC */
#define PIC2            0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)

#define PIC_EOI         0x20    /* End-of-interrupt command code */

#define ICW1_ICW4       0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */

#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */

const uint32_t CPUID_FLAG_MSR = 1 << 5;

bool cpuHasMSR()
{
   uint32_t a, b, c, d; // eax, (ebx, ecx,) edx
   __get_cpuid(1, &a, &b, &c, &d);
   return d & CPUID_FLAG_MSR;
}

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

// static bool disableLocalAPIC() {
//   if (cpuHasMSR()) {
//     cpuGetMSR
//   } else {
//     return false;
//   }
// }

static inline bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

static inline void io_wait() {

}

// from http://wiki.osdev.org/8259_PIC
static void PIC_remap(uint8_t offset1, uint8_t offset2) {
  unsigned char a1, a2;

  a1 = inb(PIC1_DATA);                        // save masks
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1);   // restore saved masks.
  outb(PIC2_DATA, a2);
}

static void init_idt() {
  debug("cpuHasMSR=%i", cpuHasMSR());
  info("Disabling APIC.");
  uint32_t msr = disable_apic();
  debug("MSR 0x1b=%x", msr);

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

  // Remap the irq table
  info("Remapping IRQs");
  PIC_remap(0x20, 0x28);
  // outb(0x20, 0x11);
  // outb(0xA0, 0x11);
  // outb(0x21, 0x20);
  // outb(0xA1, 0x28);
  // outb(0x21, 0x04);
  // outb(0xA1, 0x02);
  // outb(0x21, 0x01);
  // outb(0xA1, 0x01);
  // outb(0x21, 0x0);
  // outb(0xA1, 0x0);

  idt_set_gate(32, irq0, IDT_SELECTOR, flags);
  idt_set_gate(33, irq1, IDT_SELECTOR, flags);
  idt_set_gate(34, irq2, IDT_SELECTOR, flags);
  idt_set_gate(35, irq3, IDT_SELECTOR, flags);
  idt_set_gate(36, irq4, IDT_SELECTOR, flags);
  idt_set_gate(37, irq5, IDT_SELECTOR, flags);
  idt_set_gate(38, irq6, IDT_SELECTOR, flags);
  idt_set_gate(39, irq7, IDT_SELECTOR, flags);
  idt_set_gate(40, irq8, IDT_SELECTOR, flags);
  idt_set_gate(41, irq9, IDT_SELECTOR, flags);
  idt_set_gate(42, irq10, IDT_SELECTOR, flags);
  idt_set_gate(43, irq11, IDT_SELECTOR, flags);
  idt_set_gate(44, irq12, IDT_SELECTOR, flags);
  idt_set_gate(45, irq13, IDT_SELECTOR, flags);
  idt_set_gate(46, irq14, IDT_SELECTOR, flags);
  idt_set_gate(47, irq15, IDT_SELECTOR, flags);

  idt_flush(&idt_ptr);

  // enable hardware interrupts
  asm volatile ("sti");
  if (are_interrupts_enabled()) info("interrupts enabled.");
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

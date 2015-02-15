#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "paging.h"
#include "isr.h"
#include "string.h"
#include "memory.h"
#include "debug.h"

directory_t *page_directory; // allocated at run-time to ensure page alignment

uint32_t frame_allocations[32767];

#define PAGE_FRAME_SIZE 4096

// addr must be page aligned
static bool test_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  return  (frame_allocations[frame_word] >> bit_offset) & 1;
}

// addr must be page aligned
static void set_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  frame_allocations[frame_word] |= (1 << bit_offset);
}

// addr must be page aligned
static void clear_frame(uint32_t addr) {
  uint32_t frame = addr / PAGE_FRAME_SIZE;
  uint32_t frame_word = frame / 32;
  uint32_t bit_offset = frame % 32;
  frame_allocations[frame_word] &= ~(1 << bit_offset);
}

void handle_page_fault(registers_t regs) {
  printf("Page fault!");
}

#define PAGE_READONLY   0
#define PAGE_READWRITE  1
#define PAGE_USER       1
#define PAGE_KERNEL     0
#define PAGE_SIZE_4KB   0
#define PAGE_SIZE_4MB   1

void identity_map(uint32_t addr) {
  printf("identity mapping %x-%x\n", addr & 0xfffff000, (addr & 0xfffff000) + 0x1000);

  uint32_t directory_offset = addr >> 22; // 31:22
  uint32_t table_offset = (addr >> 12) & 0x3ff; // 21:12
  printf("\tdirectory offset: %x\n", directory_offset);
  printf("\ttable offset: %x\n", table_offset);

  directory_t *directory = &page_directory[directory_offset/4];
  page_t *table;
  printf("kernel page directory resides at %x\n", page_directory);
  if (!directory->present) {
    printf("configuring new page directory entry with index %i at %x", directory_offset, directory);
    directory->present = 1;
    directory->rw = PAGE_READWRITE;
    directory->us = PAGE_KERNEL;
    directory->ps = PAGE_SIZE_4KB;

    table = (page_t*)kmalloc_page(); // sizeof(page_t)*1024 == 0x1000
    memset(table, 0, 0x1000);

    directory->page_table = (uint32_t)table >> 12; // table base (20 high bits)
    printf("page table is now %xH\n", directory->page_table);
  } else {
    table = (page_t*)(directory->page_table << 12);
    printf("page table already exists at %x\n", table);
  }

  page_t *page = &table[table_offset/4]; // this math is wrong because table_offset is an actual address
  if (!page->present) {
    printf("configuring new page table entry at %x\n", page);
    page->present = 1;
    page->rw = PAGE_READWRITE;
    page->us = PAGE_KERNEL;
  }

  page->page_frame = addr >> 12; // frame base (20 high bits)
  printf("page %xH is now mapped is now %xH\n", addr >> 12, page->page_frame);
  set_frame(addr);
}

extern void load_page_directory(directory_t *directory);
extern void enable_paging();

void init_paging() {
  printf("initializing paging...\n");
  // identity map 0x00000000 - 0x00400000 (first 4MB) which includes kernel
  // identity map 0x00B8000 - 0x00B8000 + 80*25*2 for framebuffer
  // anything else?
  // yes, actually map range from 0 to N including all paging tables needed
  // to create that very map. this is necessary so that those tables
  // can be accessed once paging is enabled.

  page_directory = (directory_t*)kmalloc_page();
  // mark all directory entries "not present"
  memset(page_directory, 0, sizeof(directory_t)*1024);

  // starting with address 0x00000000, increment one page at a time
  // until we've identity mapped pages for all known allocated memory
  uint32_t addr;
  for (addr=0x0; addr<0x0030000; addr += 0x1000) {
    identity_map(addr);
  }

  identity_map(0x00B8000);

  // listen for page faults
  register_interrupt_handler(14, handle_page_fault);

  // finally: put &page_directory in high 20 bits of cr3 register
  // enable paging: set bit 31 (PG) of cr0 to 1.
  load_page_directory(page_directory);
  DEBUG;
  enable_paging();
  printf("paging enabled, good luck.\n");
}
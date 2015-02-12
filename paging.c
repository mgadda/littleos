#include "paging.h"

page_directory_entry_t page_directory[1024];

void init_paging() {
  // put &page_directory in high 20 bits of cr3 register
  // enable paging: set bit 31 (PG) of cr0 to 1.

}
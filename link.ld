ENTRY(loader)

. = 0xC0100000;

kernel_start = .;

SECTIONS {
  .multiboot ALIGN (0x1000): { multiboot.asm.o }

  .text ALIGN (0x1000): AT(ADDR(.text)-0xC0000000)
  {
    *(.text)
  }

  .rodata ALIGN (0x1000): AT(ADDR(.rodata)-0xC0000000)
  {
    *(.rodaata*)
  }

  .data ALIGN (0x1000): AT(ADDR(.data)-0xC0000000)
  {
    *(.data)
  }

  .bss ALIGN (0x1000): AT(ADDR(.bss)-0xC0000000)
  {
    *(COMMON)
    *(.bss)
  }

}

kernel_end = .;
ENTRY(loader)

. = 0x00100000;

SECTIONS {
  .multiboot ALIGN (0x1000): { multiboot.o }
  
  .text ALIGN (0x1000):
  {
    *(.text)
  }

  .rodata ALIGN (0x1000):
  {
    *(.rodaata*)
  }

  .data ALIGN (0x1000):
  {
    *(.data)
  }

  .bss ALIGN (0x1000):
  {
    *(COMMON)
    *(.bss)
  }

}

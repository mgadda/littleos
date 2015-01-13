ALGN         equ 1<<0
MEMINFO      equ 1<<1
MACHO        equ 1<<16
FLAGS        equ (ALGN | MEMINFO | MACHO)
MAGIC_NUMBER equ 0x1BADB002
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot:
align 4
  dd MAGIC_NUMBER
  dd FLAGS
  dd CHECKSUM



ALGN         equ 1<<0
MEMINFO      equ 1<<1
MACHO        equ 1<<16
FLAGS        equ (ALGN | MEMINFO | MACHO)
MAGIC_NUMBER equ 0x1BADB002
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

global MultiBootHeader

section .text
align 4
MultiBootHeader:
  dd MAGIC_NUMBER
  dd FLAGS
  dd CHECKSUM
  dd MultiBootHeader
  dd MultiBootHeader
  dd kernel_stack+KERNEL_STACK_SIZE
  dd kernel_stack+KERNEL_STACK_SIZE
  dd loader

extern loader
extern kernel_stack

KERNEL_STACK_SIZE equ 4096


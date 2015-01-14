global loader

ALGN         equ 1<<0
MEMINFO      equ 1<<1
FLAGS        equ (ALGN | MEMINFO)
MAGIC_NUMBER equ 0x1BADB002
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

section .multiboot:
align 4
  dd MAGIC_NUMBER
  dd FLAGS
  dd CHECKSUM
%define debug xchg bx, bx

loader:
  mov eax, 0xCAFEBABE
  ;debug
.loop:
  jmp .loop

KERNEL_STACK_SIZE equ 4096

section .bss
align 4
kernel_stack:
  resb KERNEL_STACK_SIZE
  mov esp, kernel_stack + KERNEL_STACK_SIZE

global loader
extern kernel

%define debug xchg bx, bx

section .text
loader:
  mov esp, kernel_stack + KERNEL_STACK_SIZE   ; set up stack pointer
  push ebx
  call kernel
.loop:
  jmp .loop

KERNEL_STACK_SIZE equ 4096

section .bss
align 4
kernel_stack:
  resb KERNEL_STACK_SIZE

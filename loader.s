global loader
global kernel_stack

%define debug xchg bx, bx

KERNEL_STACK_SIZE equ 4096

section .text
loader:
  mov esp, kernel_stack + KERNEL_STACK_SIZE   ; set up stack pointer
  push ebx
  call _kernel
.loop:
  jmp .loop

extern _kernel

section .bss
align 4
kernel_stack:
  resb KERNEL_STACK_SIZE


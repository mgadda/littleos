global loader

%define debug xchg bx, bx

section .text:
loader:
  mov eax, 0xCAFEBABE
  ;debug
  mov esp, kernel_stack + KERNEL_STACK_SIZE   ; set up stack pointer
  call kmain
.loop:
  jmp .loop

extern kmain


KERNEL_STACK_SIZE equ 4096

section .bss
align 4
kernel_stack:
  resb KERNEL_STACK_SIZE

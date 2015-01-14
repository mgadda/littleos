global loader

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

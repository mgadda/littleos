; setting up entry point for linker
global loader

extern kernel_main
extern boot_page_directory

%define debug xchg bx, bx

KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE.

section .text
align 4

loader equ (_loader - KERNEL_VIRTUAL_BASE)

_loader:
  ; Set up and enable paging with kernel located at 0XC0100000

  mov ecx, (boot_page_directory - KERNEL_VIRTUAL_BASE)
  mov cr3, ecx ; Load page directory base pointer

  ; enable 4MB paging
  mov eax, cr4
  or eax, 0x10
  mov cr4, eax

  ; enable paging (PG bit)
  mov eax, cr0
  or eax, 0x80000001  ; set PE (bit 0) and PG (bit 31)
  mov cr0, eax

  ; Start fetching instructions in kernel space.
  ; Since eip at this point holds the physical address of this command (approximately 0x00100000)
  ; we need to do a long jump to the correct virtual address of StartInHigherHalf which is
  ; approximately 0xC0100000.
  lea ecx, [_higher_half]
  jmp ecx


_higher_half:
  ; Unmap the identity-mapped first 4MB of physical address space. It should not
  ; be needed anymore.
  mov dword [boot_page_directory], 0
  invlpg [0]

  mov esp, kernel_stack + KERNEL_STACK_SIZE   ; set up stack pointer

  add ebx, KERNEL_VIRTUAL_BASE ; make the address virtual
  push ebx

  call kernel_main
.loop:
  jmp .loop

KERNEL_STACK_SIZE equ 0x4000 ; 16K

section .bss
align 4
kernel_stack:
  resb KERNEL_STACK_SIZE

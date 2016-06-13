global load_page_directory
global enable_paging
global boot_page_directory

PAGING_PRESENT equ 1b
PAGING_WRITABLE equ 10b
PAGING_USER_ACCESSIBLE equ 100b
PAGING_SIZE_4MB equ 10000000b

KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE.

; identity map 0x00000000 - 0x00400000 (first 4MB) which includes kernel
; and paging data structures

KERNEL_PDE_FLAGS equ PAGING_PRESENT | PAGING_WRITABLE | PAGING_SIZE_4MB

; Adapted from http://wiki.osdev.org/Higher_Half_bare_bones
section .data
align 4096
boot_page_directory:
  dd KERNEL_PDE_FLAGS ; what is this for?
  times (KERNEL_PAGE_NUMBER - 1) dd 0 ; pages before kernel page
  dd KERNEL_PDE_FLAGS ; kernel page
  times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0 ; pages after kernel page

  ; pde_frame_addr equ 0x0
  ; dd (pde_frame_addr & 0xfff00000) + (PAGING_PRESENT | PAGING_WRITABLE | PAGING_SIZE_4MB)
  ; times 0x3ff dd 0       ; allocate remaining page directory entries

; TODO: remove this, unused.
; align 4096
; boot_page_table:
; %assign frame_addr 0
; %rep 0x300
;   dd frame_addr | (PAGING_PRESENT | PAGING_WRITABLE | PAGING_USER_ACCESSIBLE)
;   %assign frame_addr frame_addr+0x1000
; %endrep
;   times 0x100 dd 0

section .text
load_page_directory:    ; put &boot_page_directory in high 20 bits of cr3 register
  mov eax, [esp+4]
  mov ebx, cr3
  and ebx, 0xfff        ; zero out existing 20 high bits
  and eax, 0xfffff000
  or ebx, eax
  mov cr3, ebx
  ret

enable_paging:
  ; enable 4MB paging
  mov eax, cr4
  or eax, 0x10
  mov cr4, eax

  ; enable paging (PG bit)
  mov eax, cr0
  or eax, 0x80000001  ; set PE (bit 0) and PG (bit 31)
  mov cr0, eax
  ret
global load_page_directory
global enable_paging
global boot_page_directory

PAGING_PRESENT equ 1b
PAGING_WRITABLE equ 10b
PAGING_USER_ACCESSIBLE equ 100b

; identity map 0x00000000 - 0x00400000 (first 3MB) which includes kernel
; and paging data structures

section .data
align 4096
boot_page_directory:
  dd boot_page_table + (PAGING_PRESENT | PAGING_WRITABLE | PAGING_USER_ACCESSIBLE)
  times 0x3ff dd 0       ; allocate remaining page directory entries

align 4096
boot_page_table:
%assign frame_addr 0
%rep 0x300
  dd frame_addr | (PAGING_PRESENT | PAGING_WRITABLE | PAGING_USER_ACCESSIBLE)
  %assign frame_addr frame_addr+0x1000
%endrep
  times 0x100 dd 0

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
  mov eax, cr0
  or eax, 0x80000001  ; set PE (bit 0) and PG (bit 31)
  xchg bx, bx
  mov cr0, eax
  ret
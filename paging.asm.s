global load_page_directory
global enable_paging

section .text
load_page_directory:
  mov eax, [esp]
  mov ebx, cr3
  and ebx, 0xfff        ; zero out 20 high bits
  and eax, 0xfffff000
  or ebx, eax
  mov cr3, ebx
  ret

enable_paging:
  xchg bx, bx
  mov eax, cr0
  or eax, 0x80000001  ; set PE (bit 0) and PG (bit 31)
  mov cr0, eax
  ret
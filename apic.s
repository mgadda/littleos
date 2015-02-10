global disable_apic

section .text
disable_apic:
  mov ecx, 0x1b
  rdmsr               ; read APIC MSR (0x1b)
  and eax, 0xfffff7ff ; set APIC enabled=0
  mov ecx, 0x1b
  wrmsr
  ret

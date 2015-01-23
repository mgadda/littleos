; from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
extern isr_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
  cli                   ; disable interrupts
  push byte 0           ; push dummy error code
  push byte %1          ; push the interrupt number
  jump isr_common_stub  ; go to common handler
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
  cli                   ; disable interrupts
  push byte %1          ; push the interrupt number
  jump isr_common_stub  ; go to common handler
%endmacro

isr_common_stub:
  pusha                 ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

  mov ax, ds            ; Lower 16-bits of eax = ds.
  push eax              ; save the data segment descriptor

  mov ax, 0x10          ; load the kernel data segment descriptor
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call isr_handler

  pop eax               ; reload the original data segment descriptor

  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popa                  ; Pops edi,esi,ebp...
  add esp, 8            ; Cleans up the pushed error code and pushed ISR number
  sti                   ; re-enable hardware interrupts
  iret                  ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP



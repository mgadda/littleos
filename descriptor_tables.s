global gdt_flush
global idt_flush

section .text
gdt_flush:          ; from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
  mov eax, [esp+4]  ; load eax with address of gdt_ptr_t
  lgdt [eax]
  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax        ; Load all data segment selectors
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump!
.flush:
  ret

idt_flush:          ; from: http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
  mov eax, [esp+4]
  lidt [eax]
  ret
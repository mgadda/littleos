// from http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
#ifndef __ISR_H__
#define __ISR_H__

typedef struct registers {
  uint32_t ds; // data segment
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
  uint32_t int_no, err_code; // interrupt number & error code
  uint32_t eip, cs, eflags, usereap, ss; // pushed by processor
} registers_t;

#endif

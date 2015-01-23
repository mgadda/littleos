#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include <stdint.h>

typedef struct idt_entry_t {
  uint16_t offset_high; // 17 - 31?
  uint8_t handle_present:1; // 16
  uint8_t privilege_level:2; // 14 - 15
  uint8_t unused:1; // 13
  uint8_t gate_size:1; // 12
  uint16_t reserved:11; // 0 - 11
  uint16_t segment_selector; // 15 - 31
  uint16_t offset_low; // 0 - 15
} idt_entry_t;


/**
 * Accepts interrupt_t struct and converts it an IDT entry
 */
void create_idt_entry() {

}
#endif /* __INTERRUPT_H_ */
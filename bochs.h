#ifndef _BOCHS_H_
#define _BOCHS_H_

/**
 * Invoke bochs debugger
 */
#define BREAK __asm__("xchg %bx,%bx")

#endif
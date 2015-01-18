#ifndef _DEBUG_H_
#define _DEBUG_H_

/**
 * Invoke bochs debugger
 */
#define DEBUG __asm__("xchg %bx,%bx")

#endif
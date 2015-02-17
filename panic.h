#ifndef __PANIC_H__
#define __PANIC_H__

#include "string.h"

#define PANIC(msg) printf("PANIC! %s at %s:%i:", msg, __FILE__, __LINE__)

#endif

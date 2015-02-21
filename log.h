#ifndef __LOG_H__
#define __LOG_H__

#include "string.h"

#if DEBUG >= 4
#define debug(msg, ...) printf("[debug] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

#if DEBUG >= 3
#define info(msg, ...) printf("[info] " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define info(msg, ...)
#endif

#if DEBUG >= 1
#define warn(msg, ...) printf("[warn] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define warn(msg, ...)
#endif

#if DEBUG >= 0
#define error(msg, ...) printf("[error] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define error(msg, ...)
#endif

#define PANIC_MSG \
  "        _                            _                            \n" \
  "       | |                     _    | |                           \n" \
  " _ _ _ | | _    ___    ____   | |_  | | _    ____   ____   ____   \n" \
  "| | | || || \\  / _ \\  / _  |  |  _) | || \\  / _  ) / ___) / _  )  \n" \
  "| | | || | | || |_| |( ( | |  | |__ | | | |( (/ / | |    ( (/ /   \n" \
  " \\____||_| |_| \\___/  \\_||_|   \\___)|_| |_| \\____)|_|     \\____)  \n" \
  "                                                                  \n" \
  "                                _                                 \n" \
  "                               | |                                \n" \
  "            ____   ___   _ _ _ | | _    ___   _   _               \n" \
  "           / ___) / _ \\ | | | || || \\  / _ \\ | | | |              \n" \
  "          ( (___ | |_| || | | || |_) )| |_| || |_| |              \n" \
  "           \\____) \\___/  \\____||____/  \\___/  \\__  |              \n" \
  "                                             (____/               \n"


#define PANIC(msg) { printf(PANIC_MSG "PANIC! %s at %s:%i\n", msg, __FILE__, __LINE__); asm volatile("cli\nhlt"); }

#endif
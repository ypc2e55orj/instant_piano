#ifndef DEBUG_H
#define DEUBG_H

#include <stdint.h>
#include "SEGGER/SEGGER_RTT.h"

void Debug_Initialize();

#ifdef __DEBUG
#define Debug_Printf(fmt, ...) SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#define Deubg_Scope(val) SEGGER_RTT_Write(1, &val, sizeof(uint32_t))
#else
#define Debug_Printf(fmt, ...)
#define Deubg_Scope(val)
#endif

#endif // DEBUG_H

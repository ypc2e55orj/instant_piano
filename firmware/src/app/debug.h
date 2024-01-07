#ifndef DEBUG_H
#define DEUBG_H

#include <stdint.h>
#include "SEGGER/SEGGER_RTT.h"

void Debug_Initialize();
#define Debug_Printf(fmt, ...) SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#define Deubg_Scope(val) SEGGER_RTT_Write(1, &val, sizeof(uint32_t))

#endif // DEBUG_H

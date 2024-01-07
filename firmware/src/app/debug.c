#include "debug.h"

#include <stdint.h> // uint32_t

#ifdef __DEBUG
uint32_t JScope_UpBuffer[4];
#endif

void Debug_Initialize()
{
#ifdef __DEBUG
  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
  SEGGER_RTT_ConfigUpBuffer(1, "JScope_u4", JScope_UpBuffer, sizeof(JScope_UpBuffer), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif
}

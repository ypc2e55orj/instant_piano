#include "ws2812.h"

#include <stdlib.h>
#include <stdbool.h>

#include "config/default/definitions.h"

// define
#define WS2812_BUFFER_SIZE (WS2812_COUNTS * WS2812_COLOR_DEPTH)

// typedef
typedef struct WS2812
{
  uint8_t outputBuffer[WS2812_BUFFER_SIZE];
} Ws2812;

// variables
static volatile Ws2812 ws2812 = {0};

// prototype
static void Ws2812_OutputIsr(TC_TIMER_STATUS status, uintptr_t context);

void Ws2812_Initialize()
{
  TC3_TimerCallbackRegister(Ws2812_OutputIsr, (uintptr_t)NULL);
}

void Ws2812_SetBuffer(uint32_t pos, uint32_t color)
{
  ws2812.outputBuffer[pos * WS2812_COLOR_DEPTH] = (color >> 8) & 0xFF;
  ws2812.outputBuffer[pos * WS2812_COLOR_DEPTH + 1] = color >> 16;
  ws2812.outputBuffer[pos * WS2812_COLOR_DEPTH + 2] = color & 0xFF;
}

void Ws2812_ClearBuffer()
{
  memset(ws2812.outputBuffer, 0, sizeof(ws2812.outputBuffer));
}

void Ws2812_Update()
{
}

static void Ws2812_OutputIsr(TC_TIMER_STATUS status, uintptr_t context)
{
}

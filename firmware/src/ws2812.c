#include "ws2812.h"

#include <string.h>
#include <stdbool.h>

#include "config/default/definitions.h"

// define
#define WS2812_BUFFER_SIZE (WS2812_COUNTS * WS2812_COLOR_DEPTH)

// typedef
struct Ws2812
{
  uint8_t outputBuffer[WS2812_BUFFER_SIZE];
};

// variables
static struct Ws2812 ws2812 = {0};

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
  const uint8_t *bufEnd = ws2812.outputBuffer + WS2812_BUFFER_SIZE;
  uint8_t *bufPtr = ws2812.outputBuffer;
  uint8_t bitMask = 0x80;

  __disable_irq();
  while (true)
  {
    Ws2812_Set();
    if (*bufPtr & bitMask)
    {
      asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; ");
    }
    else
    {
      asm volatile("nop; nop; ");
    }
    Ws2812_Clear();
    asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; ");

    if (!(bitMask >>= 1))
    {
      bitMask = 0x80;
      if (++bufPtr == bufEnd)
        break;
    }
  }
  for (int i = 0; i < 200; i++)
  {
    asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; ");
  }
  __enable_irq();
}

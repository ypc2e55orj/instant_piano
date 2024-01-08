#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>

#define WS2812_COUNTS 8
#define WS2812_COLOR_DEPTH 3

#define Ws2812_MakeColor(r, g, b) ((r) << 16 | (g) << 8 | (b))

void Ws2812_Initialize();
void Ws2812_SetBuffer(uint32_t pos, uint32_t color);
void Ws2812_ClearBuffer();
void Ws2812_Update();

#endif // WS2812_H

// libc
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// mplab x
#include "config/default/definitions.h"

enum
{
  TOY_KEYBOARD_NOTE_A_LOW,
  TOY_KEYBOARD_NOTE_AS_LOW,
  TOY_KEYBOARD_NOTE_B_LOW,
  TOY_KEYBOARD_NOTE_C,
  TOY_KEYBOARD_NOTE_CS,
  TOY_KEYBOARD_NOTE_D,
  TOY_KEYBOARD_NOTE_DS,
  TOY_KEYBOARD_NOTE_E,
  TOY_KEYBOARD_NOTE_F,
  TOY_KEYBOARD_NOTE_FS,
  TOY_KEYBOARD_NOTE_G,
  TOY_KEYBOARD_NOTE_GS,
  TOY_KEYBOARD_NOTE_A,
  TOY_KEYBOARD_NOTE_AS,
  TOY_KEYBOARD_NOTE_B,
  TOY_KEYBOARD_NOTE_C_HIGH,
  TOY_KEYBOARD_NOTE_NUM,
};
enum
{
  TOY_KEYBOARD_BUTTON_C = 0x0001,
  TOY_KEYBOARD_BUTTON_CS = 0x0002,
  TOY_KEYBOARD_BUTTON_D = 0x0004,
  TOY_KEYBOARD_BUTTON_DS = 0x0008,
  TOY_KEYBOARD_BUTTON_E = 0x0010,
  TOY_KEYBOARD_BUTTON_F = 0x0020,
  TOY_KEYBOARD_BUTTON_FS = 0x0040,
  TOY_KEYBOARD_BUTTON_G = 0x0080,
  TOY_KEYBOARD_BUTTON_GS = 0x0100,
  TOY_KEYBOARD_BUTTON_A = 0x0200,
  TOY_KEYBOARD_BUTTON_AS = 0x0400,
  TOY_KEYBOARD_BUTTON_B = 0x0800,
  TOY_KEYBOARD_BUTTON_C_HIGH = 0x1000,
};
#define TOY_KEYBOARD_MAX_SAMPLES 256
typedef struct _CHORD
{
  uint32_t waveSamples[TOY_KEYBOARD_NOTE_NUM][TOY_KEYBOARD_MAX_SAMPLES];
  uint32_t numWaveSamples[TOY_KEYBOARD_NOTE_NUM];
  uint32_t indexWaveSamples[TOY_KEYBOARD_NOTE_NUM];
  uint32_t pwmMaxDuty;
  uint16_t pressedButton;
} ToyKeyboard;


#define DEBUG_PRINT
#if defined(DEBUG_PRINT)
// SEGGER RTT
#include "SEGGER/SEGGER_RTT.h"
const char *const noteName[TOY_KEYBOARD_NOTE_NUM] = {"A ", "A#", "B ", "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B ", "C "};
int noteOffset[TOY_KEYBOARD_NOTE_NUM] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
#define DEBUG_PRINTF_INITIALIZE() SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL)
#define DEBUG_PRINTF(fmt, ...) SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF_INITIALIZE()
#define DEBUG_PRINTF(fmt, ...)
#endif

static ToyKeyboard toyKeyboard;

void ToyKeyboard_Initialize(ToyKeyboard *tk, int scale)
{
  tk->pwmMaxDuty = TCC0_PWM24bitPeriodGet();
  double interruptFreq = ((double)CPU_CLOCK_FREQUENCY / 1.0) / (double)(tk->pwmMaxDuty * 2);
  double angleFreq = 1.0 / interruptFreq * 2.0 * M_PI;
  double baseNoteFreq = 27.5 * pow(2.0, scale);

  DEBUG_PRINTF("interruptFreq: %d\r\n", (int)interruptFreq);
  DEBUG_PRINTF("pwmMaxDuty: %d\r\n", tk->pwmMaxDuty);

  for (int nNote = 0; nNote < TOY_KEYBOARD_NOTE_NUM; nNote++)
  {
    double noteFreq = (pow(exp(1.0 / 12.0 * log(2.0)), nNote) * baseNoteFreq);
    tk->numWaveSamples[nNote] = ceil(interruptFreq / noteFreq);

    DEBUG_PRINTF("%s%d: %d Hz(%d) [", noteName[nNote], scale + noteOffset[nNote], (int)noteFreq, tk->numWaveSamples[nNote]);
    for (int nSample = 0; nSample < tk->numWaveSamples[nNote]; nSample++)
    {
      double sinWave = (sin(angleFreq * nSample * noteFreq) + 1.0) / 2.0;
      tk->waveSamples[nNote][nSample] = sinWave * (double)tk->pwmMaxDuty;
      DEBUG_PRINTF("%d ", tk->waveSamples[nNote][nSample]);
    }
    DEBUG_PRINTF("]\r\n");
  }
}
uint32_t ToyKeyboard_UpdateDuty(ToyKeyboard *tk)
{
  uint32_t pwmDuty = 0, nNotes = 0;

#define CALCURATE_WAVE(note)                                                                                \
  do                                                                                                        \
  {                                                                                                         \
    if (tk->pressedButton & TOY_KEYBOARD_BUTTON_##note)                                                     \
    {                                                                                                       \
      pwmDuty += tk->waveSamples[TOY_KEYBOARD_NOTE_##note][tk->indexWaveSamples[TOY_KEYBOARD_NOTE_##note]]; \
      nNotes++;                                                                                             \
    }                                                                                                       \
  } while (0)

  CALCURATE_WAVE(C);
  CALCURATE_WAVE(D);
  CALCURATE_WAVE(E);
  CALCURATE_WAVE(F);
  CALCURATE_WAVE(G);
  CALCURATE_WAVE(A);
  CALCURATE_WAVE(B);
  CALCURATE_WAVE(C_HIGH);

#undef CALCURATE_WAVE

  if (nNotes > 0)
  {
    pwmDuty /= nNotes;
  }
  if (pwmDuty > tk->pwmMaxDuty)
  {
    pwmDuty = tk->pwmMaxDuty;
  }
  for (int i = 0; i < TOY_KEYBOARD_NOTE_NUM; i++)
  {
    if (++tk->indexWaveSamples[i] >= tk->numWaveSamples[i])
    {
      tk->indexWaveSamples[i] = 0;
    }
  }

  return pwmDuty;
}
void TCC_PeriodEventHandler(uint32_t status, uintptr_t context)
{
  TCC0_PWM24bitDutySet(TCC0_CHANNEL2, ToyKeyboard_UpdateDuty((ToyKeyboard *)context));
}
void ToyKeyboard_UpdateButton(ToyKeyboard *tk)
{
  uint16_t pressedButton = 0;
  if (ButtonC_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_C;
  }
  if (ButtonD_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_D;
  }
  if (ButtonE_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_E;
  }
  if (ButtonF_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_F;
  }
  if (ButtonG_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_G;
  }
  if (ButtonA_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_A;
  }
  if (ButtonB_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_B;
  }
  if (ButtonCn_Get())
  {
    pressedButton |= TOY_KEYBOARD_BUTTON_C_HIGH;
  }

  tk->pressedButton = pressedButton;
  if (tk->pressedButton)
  {
    YellowLed_Set();
  }
  else
  {
    YellowLed_Clear();
  }
}


int main(void)
{
  DEBUG_PRINTF_INITIALIZE();
  SYS_Initialize(NULL);

  ToyKeyboard_Initialize(&toyKeyboard, 5);

  TCC0_PWMCallbackRegister(TCC_PeriodEventHandler, (uintptr_t)&toyKeyboard);
  TCC0_PWMStart();

  RxLed_Set();
  TxLed_Set();

  while (true)
  {
    ToyKeyboard_UpdateButton(&toyKeyboard);
    SYS_Tasks();
  }

  return EXIT_FAILURE;
}

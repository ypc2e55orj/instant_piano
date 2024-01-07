// libc
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// mplab x
#include "../config/default/definitions.h"

#include "app/chord.h"
#include "app/debug.h"

enum Button
{
  BUTTON_HEAD,
  BUTTON_C = BUTTON_HEAD,
  BUTTON_D,
  BUTTON_E,
  BUTTON_F,
  BUTTON_G,
  BUTTON_A,
  BUTTON_B,
  BUTTON_CH,
  BUTTON_NUM,
};

uint16_t GetPressedButtons()
{
  uint16_t pressedButtons = 0;
  uint8_t buttons[BUTTON_NUM] = {
      ButtonC_Get(),
      ButtonD_Get(),
      ButtonE_Get(),
      ButtonF_Get(),
      ButtonG_Get(),
      ButtonA_Get(),
      ButtonB_Get(),
      ButtonCn_Get(),
  };

  for (enum Button n = BUTTON_HEAD; n < BUTTON_NUM; n++)
  {
    if (buttons[n]) {
      pressedButtons |= 1 << n;
    }
  }
  pressedButtons ? WhiteLed_Set() : WhiteLed_Clear();

  return pressedButtons;
}

uint16_t GetPressedNotes(uint16_t pressedButton)
{
  uint16_t pressedNotes = 0;
  enum Note notes[BUTTON_NUM] = {
      CHORD_NOTE_C,
      CHORD_NOTE_D,
      CHORD_NOTE_E,
      CHORD_NOTE_F,
      CHORD_NOTE_G,
      CHORD_NOTE_A,
      CHORD_NOTE_B,
      CHORD_NOTE_CH,
  };

  for (enum Button n = 0; n < BUTTON_NUM; n++)
  {
    if (pressedButton & (1 << n))
    {
      pressedNotes |= 1 << notes[n];
    }
  }

  return pressedNotes;
}

void TCC_PeriodEventHandler(uint32_t status, uintptr_t context)
{
  TCC0_PWM24bitDutySet(TCC0_CHANNEL2, Chord_CalculateDuty());
}

int main(void)
{
  uint16_t pressedButtons = 0;

  SYS_Initialize(NULL);
  Debug_Initialize();

  YellowLed_Clear();
  RxLed_Set();
  TxLed_Set();

  Chord_Initialize(CPU_CLOCK_FREQUENCY, TCC0_PWM24bitPeriodGet());
  TCC0_PWMCallbackRegister(TCC_PeriodEventHandler, (uintptr_t)NULL);

  do
  {
    pressedButtons = GetPressedButtons();
  } while (!pressedButtons);
  Chord_SetScale(__builtin_ffs(pressedButtons));
  TCC0_PWMStart();

  while (true)
  {
    Chord_SetNotes(GetPressedNotes(GetPressedButtons()));
    SYS_Tasks();
  }

  return EXIT_FAILURE;
}

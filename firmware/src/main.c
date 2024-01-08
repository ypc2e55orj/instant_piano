// libc
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// mplab x
#include "config/default/definitions.h"

#include "chord.h"

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
      CHORD_NOTE_A_H,
      CHORD_NOTE_B_H,
      CHORD_NOTE_C_H,
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

int main(void)
{
  uint16_t pressedButtons = 0;

  SYS_Initialize(NULL);

  YellowLed_Clear();
  RxLed_Set();
  TxLed_Set();

  Chord_Initialize(CPU_CLOCK_FREQUENCY, TCC0_PWM24bitPeriodGet());
  do
  {
    pressedButtons = GetPressedButtons();
  } while (!pressedButtons);
  Chord_SetScale(CHORD_FREQ_RATIO_JUST_INTONATION, __builtin_ffs(pressedButtons));
  Chord_Start();

  while (true)
  {
    Chord_SetNotes(GetPressedNotes(GetPressedButtons()));
    SYS_Tasks();
  }

  return EXIT_FAILURE;
}

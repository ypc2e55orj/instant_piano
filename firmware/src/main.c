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

#define NUM_BUTTONS 8

void UpdateButton()
{
  uint16_t pressedNotes = 0;
  uint8_t buttons[NUM_BUTTONS] = {
    ButtonC_Get(),
    ButtonD_Get(),
    ButtonE_Get(),
    ButtonF_Get(),
    ButtonG_Get(),
    ButtonA_Get(),
    ButtonB_Get(),
    ButtonCn_Get()
  };
  enum Note notes[NUM_BUTTONS] = {
    CHORD_NOTE_C,
    CHORD_NOTE_D,
    CHORD_NOTE_E,
    CHORD_NOTE_F,
    CHORD_NOTE_G,
    CHORD_NOTE_A,
    CHORD_NOTE_B,
    CHORD_NOTE_CH,
  };

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (buttons[i])
    {
      pressedNotes |= (1 << notes[i]);
    }
  }

  Chord_SetNotes(pressedNotes);

  if (pressedNotes)
  {
    WhiteLed_Set();
  }
  else
  {
    WhiteLed_Clear();
  }
}

void TCC_PeriodEventHandler(uint32_t status, uintptr_t context)
{
  TCC0_PWM24bitDutySet(TCC0_CHANNEL2, Chord_CalculateDuty());
}

int main(void)
{
  SYS_Initialize(NULL);
  Debug_Initialize();

  Chord_Initialize(CPU_CLOCK_FREQUENCY, TCC0_PWM24bitPeriodGet());
  Chord_SetScale(5);

  TCC0_PWMCallbackRegister(TCC_PeriodEventHandler, (uintptr_t)NULL);
  TCC0_PWMStart();

  YellowLed_Clear();
  RxLed_Set();
  TxLed_Set();

  while (true)
  {
    UpdateButton();
    SYS_Tasks();
  }

  return EXIT_FAILURE;
}

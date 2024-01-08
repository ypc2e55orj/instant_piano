#ifndef CHORD_H
#define CHROD_H

#include <stdint.h>

#define CHORD_MAX 3

enum FrequencyRatio
{
  CHORD_FREQ_RATIO_EQUAL_TEMPERAMENT,
  CHORD_FREQ_RATIO_JUST_INTONATION,
  CHORD_FREQ_RATIO_PYTHAGOREAN_TUNING,
  CHORD_FREQ_RATIO_NUM,
};

enum Note
{
  CHORD_NOTE_HEAD,
  CHORD_NOTE_C = CHORD_NOTE_HEAD,
  CHORD_NOTE_CS,
  CHORD_NOTE_D,
  CHORD_NOTE_DS,
  CHORD_NOTE_E,
  CHORD_NOTE_F,
  CHORD_NOTE_FS,
  CHORD_NOTE_G,
  CHORD_NOTE_GS,
  CHORD_NOTE_A_H,
  CHORD_NOTE_AS_H,
  CHORD_NOTE_B_H,
  CHORD_NOTE_C_H,
  CHORD_NOTE_NUM,
};

void Chord_Initialize(uint32_t baseClockFreq, uint32_t pwmMaxDuty);
void Chord_SetScale(enum FrequencyRatio temp, int numOctave);
void Chord_SetNotes(uint16_t notes);
uint32_t Chord_CalculateDuty();

#endif // CHROD_H

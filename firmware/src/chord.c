#include "chord.h"

#include <stdint.h>
#include <math.h>

#define CHORD_SINE_NUM_SAMPLES 8191

typedef struct _CHORD
{
  uint32_t pwmMaxDuty;
  uint32_t pwmIsrFreq;
  uint16_t sinSamples[CHORD_SINE_NUM_SAMPLES];
  uint16_t sinSkipCount[CHORD_NOTE_NUM];
  uint16_t sinSkipIndex[CHORD_NOTE_NUM];
  uint16_t notes;
} Chord;

static volatile Chord chord = {0};

void Chord_Initialize(uint32_t baseClockFreq, uint32_t pwmMaxDuty)
{
  chord.pwmMaxDuty = pwmMaxDuty;
  chord.pwmIsrFreq = baseClockFreq / (pwmMaxDuty * 2);

  for (uint32_t nSample = 0; nSample < CHORD_SINE_NUM_SAMPLES; nSample++)
  {
    double phase = sin(2.0 * M_PI * 1.0 / (double)CHORD_SINE_NUM_SAMPLES * (double)nSample);
    chord.sinSamples[nSample] = (phase + 1.0) / 2.0 * pwmMaxDuty;
  }
}

static double Chord_CalculateNoteFrequency(int numScale, enum Note note)
{
  const int noteOffset = 3;
  const double aNoteFreq = 13.75 * pow(2.0, numScale);
  return (pow(exp(1.0 / 12.0 * log(2.0)), note + noteOffset) * aNoteFreq);
}

void Chord_SetScale(int scale)
{
  for (enum Note note = CHORD_NOTE_HEAD; note < CHORD_NOTE_NUM; note++)
  {
    double noteFreq = Chord_CalculateNoteFrequency(scale, note);
    chord.sinSkipCount[note] = round(CHORD_SINE_NUM_SAMPLES * noteFreq / chord.pwmIsrFreq);
  }
}

void Chord_SetNotes(uint16_t notes)
{
  chord.notes = notes;
}

uint32_t Chord_CalculateDuty()
{
  uint32_t pwmDuty = 0;
  int nNotes = 0;
  for (enum Note note = CHORD_NOTE_HEAD; note < CHORD_NOTE_NUM; note++)
  {
    if ((chord.notes & (1 << note)))
    {
      // Button is pressed
      pwmDuty += chord.sinSamples[chord.sinSkipIndex[note]];
      nNotes++;
    }

    // Update skip index
    if ((chord.sinSkipIndex[note] += chord.sinSkipCount[note]) >= CHORD_SINE_NUM_SAMPLES)
    {
      chord.sinSkipIndex[note] = 0;
    }
  }
  if (nNotes > 0)
  {
    pwmDuty /= nNotes;
  }

  return pwmDuty;
}

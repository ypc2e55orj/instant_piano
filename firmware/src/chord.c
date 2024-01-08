#include "chord.h"

#include <stdint.h>
#include <math.h>

// define
#define CHORD_SINE_NUM_SAMPLES 8191

// typedef
typedef struct _CHORD
{
  uint32_t pwmMaxDuty;
  uint32_t pwmIsrFreq;
  uint16_t sinSamples[CHORD_SINE_NUM_SAMPLES];
  uint16_t sinSkipCount[CHORD_NOTE_NUM];
  uint16_t sinSkipIndex[CHORD_NOTE_NUM];
  uint16_t notes;
} Chord;
typedef double (*CalculateNoteFrequencyType)(int numOctave, enum Note note);

// prototype
static double Chord_CalculateNoteFrequencyEqualTemperament(int numOctave, enum Note note);
static double Chord_CalculateNoteFrequencyJustIntonation(int numOctave, enum Note note);
static double Chord_CalculateNoteFrequencyPythagoreanTuning(int numOctave, enum Note note);

// variables
static volatile Chord chord = {0};
static const CalculateNoteFrequencyType Chord_CalculateNoteFrequency[CHORD_FREQ_RATIO_NUM] = {
    Chord_CalculateNoteFrequencyEqualTemperament,
    Chord_CalculateNoteFrequencyJustIntonation,
    Chord_CalculateNoteFrequencyPythagoreanTuning,
};

static double Chord_CalculateNoteReferenceFrequency(int numOctave)
{
  return 27.5 * pow(2.0, numOctave);
}
static double Chord_CalculateNoteFrequencyEqualTemperament(int numOctave, enum Note note)
{
  const int noteOffset = 3;
  return (pow(exp(1.0 / 12.0 * log(2.0)), note + noteOffset) * Chord_CalculateNoteReferenceFrequency(numOctave));
}
static double Chord_CalculateNoteFrequencyJustIntonation(int numOctave, enum Note note)
{
  double top[CHORD_NOTE_NUM] = {1.0, 16.0, 9.0, 6.0, 5.0, 4.0, 64.0, 3.0, 8.0, 5.0, 16.0, 15.0, 2.0};
  double bottom[CHORD_NOTE_NUM] = {1.0, 15.0, 8.0, 5.0, 4.0, 3.0, 45.0, 2.0, 5.0, 3.0, 9.0, 8.0, 1.0};
  double refFreq = 6.0 / 5.0 * Chord_CalculateNoteReferenceFrequency(numOctave);
  return top[note] / bottom[note] * refFreq;
}
static double Chord_CalculateNoteFrequencyPythagoreanTuning(int numOctave, enum Note note)
{
  // not implemented yet
  return 0.0;
}

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

void Chord_SetScale(enum FrequencyRatio freqRatio, int numOctave)
{
  if (freqRatio < 0 || CHORD_FREQ_RATIO_NUM <= freqRatio)
    return;

  for (enum Note note = CHORD_NOTE_HEAD; note < CHORD_NOTE_NUM; note++)
  {
    double noteFreq = Chord_CalculateNoteFrequency[freqRatio](numOctave, note);
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

#ifndef SLICE1_V2_AUDIO_H
#define SLICE1_V2_AUDIO_H

/* NFR6: procedural retro audio — no stored samples. Square/triangle/noise
 * oscillators fill a PCM buffer deterministically (seeded), so the generation
 * is unit-testable even though playback (ALSA) is a human/ear gate. */

#include <stddef.h>
#include <stdint.h>

typedef enum { AUDIO_SQUARE, AUDIO_TRIANGLE, AUDIO_NOISE } AudioWave;

/* Fill buf with n 16-bit mono samples of a procedurally generated tone.
 * noise_state may be NULL for non-noise waves (deterministic from i). */
void audio_fill(int16_t *buf, size_t n, double freq, AudioWave wave,
                int sample_rate, uint32_t *noise_state);

int audio_selftest(int verbose);

#endif

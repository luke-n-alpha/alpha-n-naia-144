#include "audio.h"

#include <math.h>
#include <stdio.h>

#define AUDIO_AMP 12000

void audio_fill(int16_t *buf, size_t n, double freq, AudioWave wave,
                int sample_rate, uint32_t *noise_state) {
    size_t i;
    for (i = 0; i < n; ++i) {
        double phase = freq * (double)i / (double)sample_rate;
        double frac = phase - floor(phase);
        int16_t s = 0;
        switch (wave) {
            case AUDIO_SQUARE:
                s = frac < 0.5 ? (int16_t)AUDIO_AMP : (int16_t)(-AUDIO_AMP);
                break;
            case AUDIO_TRIANGLE:
                s = (int16_t)((double)AUDIO_AMP * (4.0 * fabs(frac - 0.5) - 1.0));
                break;
            case AUDIO_NOISE: {
                uint32_t st = noise_state ? *noise_state : 1u;
                st = st * 1103515245u + 12345u;
                if (noise_state) *noise_state = st;
                s = (int16_t)(((int)((st >> 16) & 0xFFFFu) - 32768) / 3);
                break;
            }
        }
        buf[i] = s;
    }
}

int audio_selftest(int verbose) {
    int16_t buf[32];
    uint32_t ns = 7u;
    int ok = 1;

    /* square @ 440Hz, 8000sr: phase0 -> +AMP; near half-period -> -AMP */
    audio_fill(buf, 32, 440.0, AUDIO_SQUARE, 8000, NULL);
    if (buf[0] != AUDIO_AMP) ok = 0;
    /* 8000/440 ~= 18.2 samples/period; sample 10 is past half period -> -AMP */
    if (buf[10] != (int16_t)(-AUDIO_AMP)) ok = 0;

    /* triangle starts at trough (-AMP at frac 0) */
    audio_fill(buf, 32, 440.0, AUDIO_TRIANGLE, 8000, NULL);
    if (buf[0] != (int16_t)((double)AUDIO_AMP * (4.0 * 0.5 - 1.0))) ok = 0;  /* = +AMP at frac0 */

    /* noise reproducible from seed */
    {
        int16_t a[4], b[4];
        uint32_t s1 = ns, s2 = ns;
        audio_fill(a, 4, 0.0, AUDIO_NOISE, 8000, &s1);
        audio_fill(b, 4, 0.0, AUDIO_NOISE, 8000, &s2);
        if (a[0] != b[0] || a[3] != b[3]) ok = 0;
    }

    if (verbose) {
        printf("audio_square0=%d\n", buf[0]);
    }
    printf("audio_selftest=%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}

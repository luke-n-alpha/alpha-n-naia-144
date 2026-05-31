/* NFR6 deterministic acceptance: procedural audio generation is unit-testable
 * (square/triangle phase + seeded noise reproducibility). Playback = human ear. */
#include "audio.h"

int main(void) {
    return audio_selftest(1);
}

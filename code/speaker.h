#ifndef SPEAKER_H
#define SPEAKER_H

#include <msp430.h>
#include <stdint.h>

void speaker_init(void);
void speaker_tone_begin(uint16_t freq);
void speaker_tone_end(uint16_t freq);

#endif


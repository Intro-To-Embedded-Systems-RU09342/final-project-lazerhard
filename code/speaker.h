#ifndef SPEAKER_H
#define SPEAKER_H

#include <msp430.h>
#include <stdint.h>

void speaker_init(void);
void speaker_set_octave(uint8_t octave);
void speaker_set_volume(uint8_t note, uint8_t volume);
uint16_t speaker_get_volume(uint8_t note);

#endif


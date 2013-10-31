#ifndef _SOUND_H_
#define _SOUND_H_

#include <stdbool.h>

extern bool sound_command;

void SoundInit(uint16_t sum_samples);
void SoundFillUsb();

#endif//_SOUND_H_

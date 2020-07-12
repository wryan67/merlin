#pragma once
#include <stdint.h>
#include <alsa/asoundlib.h>

#include "Sound.h"


void playTone(snd_pcm_t* soundCardHandle, float freq, float duration, wavHeaderType* wavHeader);

void generate_sine(unsigned char* data, snd_pcm_format_t format,
    wavHeaderType* wavHeader, snd_pcm_uframes_t offset,
    int count, double* _phase, float freq);



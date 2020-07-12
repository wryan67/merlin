#include "tones.h"
#include <math.h>

void generate_sine(unsigned char* data, snd_pcm_format_t format,
    wavHeaderType* wavHeader, snd_pcm_uframes_t offset,
    int count, double* _phase, float freq) {

    static double max_phase = 2. * M_PI;
    double phase = *_phase;
    double step = max_phase * freq / (double)wavHeader->sampleRate;
    int format_bits = snd_pcm_format_width(format);
    unsigned int maxval = (1 << (format_bits - 1)) - 1;
    int bps = format_bits / 8;  /* bytes per sample */
    int phys_bps = snd_pcm_format_physical_width(format) / 8;
    int big_endian = snd_pcm_format_big_endian(format) == 1;
    int to_unsigned = snd_pcm_format_unsigned(format) == 1;
    int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
        format == SND_PCM_FORMAT_FLOAT_BE);

    for (int k = 0; k < count; ++k) {
        union {
            float f;
            int i;
        } fval;
        int res, i;
        if (is_float) {
            fval.f = sin(phase);
            res = fval.i;
        }
        else {
            res = sin(phase) * maxval;
        }
        if (to_unsigned)
            res ^= 1U << (format_bits - 1);
        for (int chn = 0; chn < wavHeader->channels; chn++) {
            long wavIndex = k * wavHeader->bitsPerSample / 8 + chn * wavHeader->bitsPerSample / 8;
            /* Generate data in native endian format */
            if (!big_endian) {
                for (i = 0; i < bps; i++)
                    *(data + k + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
            }
            else {
                for (i = 0; i < bps; i++)
                    *(data + k + i) = (res >> i * 8) & 0xff;
            }
        }
        phase += step;
        if (phase >= max_phase)
            phase -= max_phase;
    }
    *_phase = phase;
}


void sendHeader(snd_pcm_t* soundCardHandle, wavHeaderType* wavHeader) {
    int err;

    snd_pcm_drop(soundCardHandle);

    if ((err = snd_pcm_set_params(soundCardHandle,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1,  // channels
        wavHeader->sampleRate,  // sps
        0,  // software resample
        500000)) < 0) {   /* latency 0.5sec */

        fprintf(stderr, "Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void drainSound(snd_pcm_t* soundCardHandle) {
    int err = snd_pcm_drain(soundCardHandle);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_drain failed: %s\n", snd_strerror(err));
    }
}

void playTone(snd_pcm_t* soundCardHandle, float freq, float duration, wavHeaderType* wavHeader) {
    long dataSize = wavHeader->blockAlign * wavHeader->sampleRate * duration;

    void* data = malloc(dataSize);
    memset(data, 0, dataSize);

    chunkHeader chunkHeader;
    strncpy(chunkHeader.chunkID, "data", 4);
    chunkHeader.chunkSize = dataSize;


    long samples = wavHeader->sampleRate * duration;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    double phase = 1.0;
    if (freq > 0) {
        generate_sine((unsigned char*)data, format, wavHeader, 0, samples, &phase, freq);
    }

    sendHeader(soundCardHandle, wavHeader);

    int segmetSize = (wavHeader->bitsPerSample / 8) * wavHeader->channels;

    snd_pcm_sframes_t bytesWritten;
    bytesWritten = snd_pcm_writei(soundCardHandle, data, dataSize / segmetSize);

    if (bytesWritten < 0) {
        fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(bytesWritten));
        exit(EXIT_FAILURE);
    }

    if (bytesWritten > 0 && bytesWritten < dataSize / segmetSize) {
        fprintf(stderr, "data write error (expected %li, wrote %li)\n", dataSize, bytesWritten);
    }

    drainSound(soundCardHandle);


    free(data);
}




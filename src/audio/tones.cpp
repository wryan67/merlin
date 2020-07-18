#include "Sound.h"
#include "Common.h"

void generate_sine(unsigned char* data, wavFormatType &wavConfig, int samples, double &phase, float freq) {
    static double max_phase = 2.0 * M_PI;
    double step = max_phase * freq / wavConfig.sampleRate;
    
    unsigned int maxVolume = (1 << (wavConfig.bitsPerSample - 1)) - 1;  

    int bps = wavConfig.bitsPerSample / 8;  /* bytes per sample */

    long wavIndex = 0;
    for (long sample = 0; sample < samples; ++sample) {
        int res = sin(phase) * maxVolume;

        for (int channel = 0; channel < wavConfig.channels; channel++) {
            wavIndex = sample * wavConfig.blockAlign + channel * wavConfig.blockAlign;

            /* Generate data in little endian format */
            for (int i = 0; i < bps; i++) {
                *(data + sample + wavConfig.blockAlign - 1 - i) = (res >> i * 8) & 0xff;
            }
        }
        phase += step;
        if (phase >= max_phase) {
            phase -= max_phase;
        }
    }

}




void playTone(float freq, float duration, wavFormatType &wavConfig) {
    snd_pcm_t* handle = getSoundCardHandle();
    if (handle == NULL) {
        fprintf(stderr, "soundCardHandle is null\n"); fflush(stderr);
        return;
    }
    sendWavConfig(handle, wavConfig);
    double phase = 0;
    _playTone(handle, freq, duration, wavConfig, phase);
    closeSoundCard(handle);
}


void _playTone(snd_pcm_t* soundCardHandle, float freq, double duration, wavFormatType &wavConfig, double &phase) {

    long samples  = (wavConfig.sampleRate * duration) + 0.5;
    long dataSize = samples * wavConfig.blockAlign * wavConfig.channels;
    long frames = dataSize / (wavConfig.bitsPerSample * wavConfig.channels / 8);

//    printWavConfig(wavConfig);
//    printf("duration=%lf \nsamples=%ld  \ndataSize=%ld  \nframes=%ld\n", 
//            duration,      samples,      dataSize,        frames); 
//    fflush(stdout);

    void* data = malloc(dataSize * 5);
    memset(data, 0, dataSize * 5);

    if (freq > 0) {
        generate_sine((unsigned char*)data, wavConfig, samples+samples, phase, freq);
    }

    snd_pcm_sframes_t framesWritten;
    framesWritten = snd_pcm_writei(soundCardHandle, data, frames);

    if (framesWritten < 0) {
        fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(framesWritten));
        exit(EXIT_FAILURE);
    }

    if (framesWritten > 0 && framesWritten < samples) {
        fprintf(stderr, "data write error (expected %ld, wrote %ld)\n", dataSize, (long)framesWritten);
    }

    free(data);
}


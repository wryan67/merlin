#include "Sound.h"


snd_pcm_t* openSoundCard(const char* soundCardName) {
    int err;

    if (strlen(soundCardName) == 0) {
        soundCardName = "default";
    }

    snd_pcm_t* soundCardHandle;

    if ((err = snd_pcm_open(&soundCardHandle, soundCardName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    return soundCardHandle;
}

void drainSound(snd_pcm_t* soundCardHandle) {
    int err = snd_pcm_drain(soundCardHandle);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_drain failed: %s\n", snd_strerror(err));
    }
}

void closeSoundCard(snd_pcm_t* soundCardHandle) {
    drainSound(soundCardHandle);
    int err = snd_pcm_close(soundCardHandle);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_close failed: %s\n", snd_strerror(err));
    }
}

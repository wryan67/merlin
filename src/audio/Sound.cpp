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
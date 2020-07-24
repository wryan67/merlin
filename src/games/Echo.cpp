#include "Echo.h"
#define SND_PCM_BLOCK 0

namespace Games {
    Echo::Echo() {
        gameName = "Echo";
        gameWav = "echo.wav";

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            keyTonesAudible[i] = false;
        }
    }



    void Echo::restartGame() {
        printf("%s\n", gameName);
        interruptFlag = false;
        clearBoard();
        song.clear();
        isActive = true;
    }

    void Echo::printState(snd_pcm_t* handle, const char* description) {
        const char* msg;

        snd_pcm_state_t state = snd_pcm_state(handle);
        switch (state) {
        case SND_PCM_STATE_OPEN:            msg = "SND_PCM_STATE_OPEN"; break;
        case SND_PCM_STATE_SETUP:           msg = "SND_PCM_STATE_SETUP"; break;
        case SND_PCM_STATE_PREPARED:        msg = "SND_PCM_STATE_PREPARED"; break;
        case SND_PCM_STATE_RUNNING:         msg = "SND_PCM_STATE_RUNNING"; break;
        case SND_PCM_STATE_XRUN:            msg = "SND_PCM_STATE_XRUN"; break;
        case SND_PCM_STATE_DRAINING:        msg = "SND_PCM_STATE_DRAINING"; break;
        case SND_PCM_STATE_PAUSED:          msg = "SND_PCM_STATE_PAUSED"; break;
        case SND_PCM_STATE_SUSPENDED:       msg = "SND_PCM_STATE_SUSPENDED"; break;
        case SND_PCM_STATE_DISCONNECTED:    msg = "SND_PCM_STATE_DISCONNECTED"; break;
        default:
            msg = "unknown";
        }
        printf("sound card state @%s: %s\n", description, msg);
    }

    void Echo::keypadButtonReleased(int button, long long elapsed) {
        if (debug) fprintf(stderr, "Echo -- key pressed:  %d\n", button);
        clearBoard();
        render();
        if (elapsed < 200) {
            return;
        }
        song.push_back(button);
        keyTone(button);
    }

    void Echo::computerTurn() {
        if (debounce(HIT_ME, 100)) return;

        if (isPlaying) {
            interruptFlag = true;
            usleep(410 * 1000);
            interruptFlag = false;            
            isActive = true;
            isPlaying = false;
            clearBoard();
            return;
        }
        isActive = false;
        isPlaying = true;

        snd_pcm_t* handle = getSoundCardHandle();
        if (handle == NULL) {
            fprintf(stderr, "soundCardHandle is null\n"); fflush(stderr);
            return;
        }
        int err=snd_pcm_nonblock(handle, SND_PCM_BLOCK);  // this is the default mode
        if (err != 0) {
            fprintf(stderr, "unable to set blocking mode on sound card\n"); fflush(stdout);
        }
        sendWavConfig(handle, wavConfig);
        double phase = 1.0;

        clearBoard();
        for (int i : song) {
            if (interruptFlag || isActive) {
                isPlaying = false;
                isActive = true;
                closeSoundCard(handle);
                return;
            }

            _playTone(handle, noteHz[i], .310, wavConfig, phase);
            setPixelColor(i, keyFlashColor);    render();
            delay(200);

            _playTone(handle, 0,         .100, wavConfig, phase);
            setPixelColor(i, -1);   render();
        }

        closeSoundCard(handle);

        render();
        isActive = true;
        isPlaying = false;
    }
}

#include "Echo.h"

namespace Games {
    Echo::Echo() {
        gameName = "Echo";
        gameWav = "echo.wav";


    }

    void Echo::restartGame() {
        printf("%s\n", gameName);
        interruptFlag = false;
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            pixelColor[i] = OFF;
        }
        render();
        song.clear();
        isActive = true;
    }

    void Echo::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Echo -- key pressed:  %d\n", button);
        pixelColor[pixelMap[button]] = OFF;
        render();
        song.push_back(button);
    }

    void Echo::computerTurn() {
        isActive = false;
        for (int i : song) {
            if (interruptFlag || isActive) {
                return;
            }
            playTone(globalSoundCardHandle, noteHz[i], .333, &wavHeader);
            usleep(1000);
        }
        isActive = true;
    }
}

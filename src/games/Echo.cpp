#include "Echo.h"

namespace Games {
    Echo::Echo() {
        gameName = "Echo";
        gameWav = "echo.wav";
    }

    void Echo::clearBoard() {
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            setPixel(i, -1);
        }
        render();
    }

    void Echo::restartGame() {
        printf("%s\n", gameName);
        interruptFlag = false;
        clearBoard();
        song.clear();
        isActive = true;
    }

    void Echo::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Echo -- key pressed:  %d\n", button);
        setPixel(button, -1);
        render();
        song.push_back(button);
    }

    void Echo::computerTurn() {
        if (isPlaying) {
            interruptFlag = true;
            usleep(333 * 1000);
            interruptFlag = false;            
            isActive = true;
            isPlaying = false;
            clearBoard();
            return;
        }
        isActive = false;
        isPlaying = true;
        for (int i : song) {
            if (interruptFlag || isActive) {
                return;
            }
            setPixel(i, RED);
            render();
            playTone(globalSoundCardHandle, noteHz[i], .333, &wavHeader);
            usleep(1000);
            setPixel(i, -1);
        }

        render();
        isActive = true;
        isPlaying = false;
    }
}

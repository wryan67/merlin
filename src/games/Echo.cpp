#include "Echo.h"

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

    void Echo::keypadButtonReleased(int button, long long elapsed) {
        if (debug) fprintf(stderr, "Echo -- key pressed:  %d\n", button);
        setPixelColor(button, -1);
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
                isPlaying = false;
                isActive = true;
                return;
            }
            setPixelColor(i, keyFlashColor);
            render();
            playTone(noteHz[i], .333, &wavForamt);
            usleep(1000);
            setPixelColor(i, -1);
            render();
        }

        render();
        isActive = true;
        isPlaying = false;
    }
}

#include "CodeBreaker.h"

namespace Games {
    CodeBreaker::CodeBreaker() {
        gameName = "Code Breaker";
        gameWav = "codebreaker.wav";
    }


    void CodeBreaker::restartGame() {
        printf("%s\n", gameName);
        clearBoard();
        eSpeak("under construction");
        isActive = true;
    }

    void CodeBreaker::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Code Breaker -- key pressed:  %d\n", button);
    }

}
#include "CodeBreaker.h"
#include <algorithm>


namespace Games {
    CodeBreaker::CodeBreaker() {
        gameName = "Code Breaker";
        gameWav = "codebreaker.wav";
    }
    void CodeBreaker::announceGame() {
        clearBoard();
        playWav(gameWav, false);
    }

    void CodeBreaker::announceSameGame() {
        clearBoard();
        playWav("samegame.wav", false);
    }

    void CodeBreaker::restartGame() {
        printf("%s\n", gameName);
        clearBoard();
        eSpeak("under construction");
        isActive = true;
    }

    void CodeBreaker::render() {
        if (debug) fprintf(stderr, "render: ");

        clearBoard();

        if (codeLength() < 1) {
            return;
        }
//      setPixelColor(card, playerColor);

        GameEngine::render();
    }

    int CodeBreaker::codeLength() {
        return secrectCode.size();
    }

    void CodeBreaker::generateCode(int codeLength) {
        secrectCode.clear();
        vector<int> randomCodes;

        randomCodes.clear();
        for (int i = 1; i <= 9; ++i) {
            randomCodes.push_back(i);
        }

        random_shuffle(randomCodes.begin(), randomCodes.end());

        for (int i = 0; i < codeLength; ++i) {
            int code = randomCodes.back();
            randomCodes.pop_back();

            secrectCode.push_back(code);
        }
        printVector("secrectCode", secrectCode);

    }

    void CodeBreaker::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Code Breaker -- key pressed:  %d\n", button);
    }

}
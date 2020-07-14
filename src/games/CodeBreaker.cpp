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
        secrectCode.clear();
        playerGuesses.clear();
        displayGuesses.clear();
        
        eSpeak("select seecrect code length");
        printf("espeak directions done\n"); fflush(stdout);
        isActive = true;
    }

    void CodeBreaker::render() {
        if (debug) fprintf(stderr, "render: ");

        clearBoard();

        if (codeLength() < 1) {
            return;
        }

        for (pair<int, int> e : displayGuesses) {
            setPixelColor(e.first, e.second);
        }

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

    bool CodeBreaker::vectorContains(vector<int> haystack, int needle) {
        for (int i : haystack) {
            if (i == needle) {
                return true;
            }
        }
        return false;
    }

    void CodeBreaker::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Code Breaker -- key pressed:  %d\n", button);

        render();
        if (codeLength() < 1) {
            if (button < 1 || button>10) {
                playWav("buzzer.wav", true);
                return;
            }
            generateCode(button);
            char message[256];
            sprintf(message, "begin guessing");
            eSpeak(message);
            return;
        }

        playerGuesses.push_back(button);
        if (playerGuesses.size() < codeLength()) {
            return;
        }

        bool correct = true;
        for (int i = 0; i < codeLength(); ++i) {
            if (playerGuesses[i] == secrectCode[i]) {
                displayGuesses[i] = playerColor;
            } else {
                correct = false;
                if (vectorContains(secrectCode, i)) {
                    displayGuesses[i] = computerColor;
                }
            }
        }

        render();
        if (correct) {
            isActive = false;
            playWav("achievement-00.wav", true);
        } else {
            playWav("incorrect.wav", true);
            playerGuesses.clear();
        }
    }


}
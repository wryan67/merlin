#include "MagicSquare.h"

namespace Games {
    MagicSquare::MagicSquare() {
        gameName = "Magic Square";
        gameWav = "magicsquare.wav";
    }

    void MagicSquare::randomizeBoard() {
        for (int i = 1; i <= 9; ++i) {
            pixelState[i] = random(0, 1);
        }
        render();
    }

    void MagicSquare::restartGame() {
        printf("%s\n", gameName);

        randomizeBoard();
        isActive = true;
    }

    void MagicSquare::keypadButtonReleased(int button, long long elapsed) {
        fprintf(stderr, "Magic Square -- key pressed:  %d   elapsed=%lld\n", button, elapsed);

        render();
        if (elapsed < 200) {
            return;
        }
        swapKey(button);
    }

    void MagicSquare::swapState(int i) {
        if (pixelState[i] == 1) {
            pixelState[i] = 0;
        }
        else {
            pixelState[i] = 1;
        }
    }

    void MagicSquare::render() {
        for (int i = 1; i <= 9; ++i) {
            setPixelColor(i, (pixelState[i])?playerColor:computerColor);
        }
        pixelColor[pixelMap[0]] = OFF;
        pixelColor[pixelMap[10]] = OFF;

        GameEngine::render();
    }

    void MagicSquare::swapKey(int i) {
        if (debug) printf("Magic Square: keypressed=%d\n",i);

        switch (i) {
        case 1:  swapState(1); swapState(2); swapState(4); swapState(5); break;
        case 3:  swapState(2); swapState(3); swapState(5); swapState(6); break;
        case 7:  swapState(7); swapState(8); swapState(4); swapState(5); break;
        case 9:  swapState(8); swapState(9); swapState(5); swapState(6); break;

        case 2:  swapState(1); swapState(2); swapState(3); break;
        case 4:  swapState(1); swapState(4); swapState(7); break;
        case 6:  swapState(3); swapState(6); swapState(9); break;
        case 8:  swapState(7); swapState(8); swapState(9); break;

        case 5:  swapState(5); swapState(2); swapState(4); swapState(6); swapState(8); break;
        //default: pixelColor[pixelMap[i]] = 0;

        }
        render();
        if (pixelState[1] == 1 &&
            pixelState[2] == 1 &&
            pixelState[3] == 1 &&
            pixelState[4] == 1 &&
            pixelState[5] == 0 &&
            pixelState[6] == 1 &&
            pixelState[7] == 1 &&
            pixelState[8] == 1 &&
            pixelState[9] == 1
            ) {
            isActive = false;
            playAchivement();
        }
    }
}
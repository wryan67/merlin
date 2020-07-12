#include "MagicSquare.h"

namespace Games {
    MagicSquare::MagicSquare() {
        gameName = "Magic Square";
        gameWav = "magicsquare.wav";


    }

    void MagicSquare::randomizeBoard() {
        pixelColor[pixelMap[0]] = OFF;
        for (int i = 1; i <= 9; ++i) {
            pixelState[i] = random(0, 1);
            pixelColor[pixelMap[i]] = (pixelState[i] == 0) ? neopixel_wheel(BLUE) : neopixel_wheel(GREEN);
        }
        pixelColor[pixelMap[10]] = OFF;
        render();
    }

    void MagicSquare::restartGame() {
        fprintf(stderr, "Magic Square\n");

        randomizeBoard();
        isActive = true;
    }

    void MagicSquare::keypadPressed(int button) {
        fprintf(stderr, "Magic Square -- key pressed:  %d\n", button);
        swapKey(button);
    }

    void MagicSquare::swapKey(int i) {
        //vector<int> todo;
        printf("ms:keypressed=%d\n",i);
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
        default: pixelColor[pixelMap[i]] = 0;

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
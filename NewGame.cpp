#include "NewGame.h"
#include "Common.h"

using namespace std;

namespace Games {
    NewGame::NewGame(): Game() {
        gameName = "New Game Selector";
    }

    void NewGame::restartGame() {
        fprintf(stderr, "New Game Selector\n");
        pixelState[0] = 0;
        pixelColor[pixelMap[0]] = 0;
        for (int i = 1; i <= 6; ++i) {
            pixelState[i] = 0;
            pixelColor[pixelMap[i]] = neopixel_wheel(GREEN);
        }
        for (int i = 7; i <= 10; ++i) {
            pixelState[i] = 0;
            pixelColor[pixelMap[i]] = OFF;
        }

        render();
        isActive = true;
    }

    void NewGame::keypadPressed(int button) {
        if (button >= 0 && button<=6) {
            startGame(button);
        }
    }

}
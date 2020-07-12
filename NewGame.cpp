#include "NewGame.h"
#include "Common.h"

using namespace std;

namespace Games {
    NewGame::NewGame(): GameEngine() {
        gameName = "New Game";
        gameWav = "newgame.wav";
    }


    void NewGame::resetGameBoard() {
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            if (enabledGames[i]) {
                pixelState[i] = 0;
                pixelColor[pixelMap[i]] = neopixel_wheel(CYAN);
                keyTonesAudible[5] = false;
            }
            else {
                pixelState[0] = 0;
                pixelColor[pixelMap[i]] = OFF;
            }
        }
        render();
    }
    void NewGame::restartGame() {
        fprintf(stderr, "New Game Selector\n");

        resetGameBoard();
        isActive = true;
    }

    void NewGame::keypadButtonReleased(int button) {
        resetGameBoard();

        switch (button) {
        case 5:
            startGame(button);
            break;
        }
    }

}
#include "NewGame.h"

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
                keyTonesAudible[i] = false;
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

    void NewGame::keypadButtonReleased(int button, long long elapsed) {
        resetGameBoard();

        if (enabledGames[button]) {
            startGame(button);
        }
    }

}

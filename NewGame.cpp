#include "NewGame.h"
#include "Common.h"

using namespace std;

namespace Games {
    NewGame::NewGame(): GameEngine() {
        gameName = "New Game";
        gameWav = "newgame.wav";

        keyTonesAudible[5] = false;

    }

    void NewGame::restartGame() {
        fprintf(stderr, "New Game Selector\n");

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            switch (i) {
            case 5:
                pixelState[i] = 0;
                pixelColor[pixelMap[i]] = neopixel_wheel(CYAN);
                break;
            default:
                pixelState[0] = 0;
                pixelColor[pixelMap[i]] = OFF;
            }
        }

        
        render();
        isActive = true;
    }

    void NewGame::keypadPressed(int button) {
        switch (button) {
        case 5:
            startGame(button);
            break;
        }
    }

}
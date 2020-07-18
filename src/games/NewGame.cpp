#include "NewGame.h"

using namespace std;

namespace Games {
    NewGame::NewGame(): GameEngine() {
        gameName = "New Game";
        gameWav = "newgame.wav";
    }


    void NewGame::render() {
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
        GameEngine::render();
    }
    void NewGame::restartGame() {
        fprintf(stderr, "New Game Selector\n");

        render();
        isActive = true;
    }

    void NewGame::keypadButtonReleased(int button, long long elapsed) {
        render();
        if (elapsed < 250) {
            return;
        }
        
        if (button == 0) {
            screenReader = !screenReader;
            if (screenReader) {
                eSpeak("screen reader on");
            } else {
                eSpeak("screen reader off");
            }
        }

        if (enabledGames[button]) {
            startGame(button);
        }
    }

}

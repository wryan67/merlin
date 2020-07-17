#pragma once
#include "GameEngine.h"

namespace Games {
    class MagicSquare : public GameEngine {
    protected: 
        void keypadButtonReleased(int button, long long elapsed);
        void swapState(int i);
        void render();
        void swapKey(int i);
        void randomizeBoard();
        void restartGame();

    public:
        MagicSquare();
    };
}


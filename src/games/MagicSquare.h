#pragma once
#include "GameEngine.h"

namespace Games {
    class MagicSquare : public GameEngine {
    protected: 
        void keypadButtonReleased(int button);
        void swapState(int i);
        void swapKey(int i);
        void randomizeBoard();
        void restartGame();

    public:
        MagicSquare();
    };
}


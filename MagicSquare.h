#pragma once
#include "GameEngine.h"

namespace Games {
    class MagicSquare : public GameEngine {
    protected: 
        void keypadPressed(int button);
        void swapKey(int i);
        void randomizeBoard();
        void restartGame();

    public:
        MagicSquare();
    };
}


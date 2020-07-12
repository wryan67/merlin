#pragma once
#include "Game.h"

namespace Games {
    class MagicSquare : public Game {
    protected: 
        void keypadPressed(int button);
        void swapKey(int i);
        void randomizeBoard();
        void restartGame();

    public:
        MagicSquare();
    };
}


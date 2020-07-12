#pragma once
#include "GameEngine.h"

namespace Games {
    class TicTacToe : public GameEngine {
    protected: 
        void keypadButtonReleased(int button);
        void swapKey(int i);
        void randomizeBoard();
        void restartGame();

    public:
        TicTacToe();
    };
}


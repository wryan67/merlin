#pragma once
#include "GameEngine.h"

namespace Games {
    class Echo : public GameEngine {
    private:
        vector<int> song;

    protected: 
        void keypadButtonReleased(int button);
        void computerTurn();
        void randomizeBoard();
        void restartGame();


    public:
        Echo();
    };
}


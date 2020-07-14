#pragma once
#include "GameEngine.h"

namespace Games {
    class Echo : public GameEngine {
    private:
        vector<int> song;
        bool isPlaying;

    protected: 
        void keypadButtonReleased(int button);
        void computerTurn();
        void restartGame();

    public:
        Echo();
        
    };
}


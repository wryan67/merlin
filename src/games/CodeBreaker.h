#pragma once
#include "GameEngine.h"

namespace Games {
    class CodeBreaker : public GameEngine {
    protected: 
        void keypadButtonReleased(int button);
        void restartGame();

    public:
        CodeBreaker();
        void announceGame();
        void announceSameGame();
    };
}


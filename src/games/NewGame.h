#pragma once
#include "GameEngine.h"

namespace Games {
    class NewGame : public GameEngine {

    public:
        NewGame();

        void resetGameBoard();

        void restartGame();
        void keypadButtonReleased(int button);
    };

}


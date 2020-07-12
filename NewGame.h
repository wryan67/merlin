#pragma once
#include "GameEngine.h"

namespace Games {
    class NewGame : public GameEngine {

    public:
        NewGame();

        void restartGame();
        void keypadPressed(int button);
    };

}


#pragma once
#include "Game.h"

namespace Games {
    class NewGame : public Game {

    public:
        NewGame();

        void restartGame();
        void keypadPressed(int button);
    };

}


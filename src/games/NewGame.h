#pragma once
#include "GameEngine.h"

namespace Games {
    class NewGame : public GameEngine {
    private:
        bool enabledGames[MERLIN_LIGHTS] = {
            //          0      1      2      3      4      5     6      7      8      9      10
                        false, true, true, false, false, true, false, false, false, false, false
        };


    public:
        NewGame();

        void resetGameBoard();

        void restartGame();
        void keypadButtonReleased(int button);
    };

}


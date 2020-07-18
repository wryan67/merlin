#pragma once
#include "GameEngine.h"

namespace Games {
    class NewGame : public GameEngine {
    private:
        bool enabledGames[MERLIN_LIGHTS] = {
            //          0      1     2     3     4     5     6     7      8      9      10
                        false, true, true, true, true, true, true, false, false, false, true
        };


    public:
        NewGame();

        void render();

        void restartGame();
        void keypadButtonReleased(int button, long long elapsed);
    };

}


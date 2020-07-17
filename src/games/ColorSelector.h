#pragma once
#include "GameEngine.h"

namespace Games {
    class ColorSelector : public GameEngine {
        enum CustomColors {
            PLAYER, COMPUTER, FLASH
        };
        unordered_map<int, int> customColors;
        unordered_map<int, int> palletMap;

    protected: 
        bool isTaken(int button);
        void keypadButtonReleased(int button, long long elapsed);
        void restartGame();

    public:
        ColorSelector();
        void announceGame();
        void announceSameGame();
    };
}


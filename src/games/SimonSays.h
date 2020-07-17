#pragma once
#include "GameEngine.h"

namespace Games {
    class SimonSays : public GameEngine {
    private:
        vector<int> computerSequence;

        vector<int> humanSequence;
        bool isPlaying = false;

    protected: 
        void keypadButtonReleased(int button, long long elapsed);
        void computerTurn();
        void GenerateNextSequence();
        void restartGame();
        
    public:
        SimonSays();
        void announceGame();
        void announceSameGame();
    };
}


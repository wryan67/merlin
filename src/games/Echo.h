#pragma once
#include "GameEngine.h"

namespace Games {
    class Echo : public GameEngine {
    private:
        vector<int> song;
        bool isPlaying;

    protected: 
        void keypadButtonReleased(int button, long long elapsed);
        void computerTurn();
        void restartGame();
        void printState(snd_pcm_t* handle, const char *description);
    public:
        Echo();
        
    };
}


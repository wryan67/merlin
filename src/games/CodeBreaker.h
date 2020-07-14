#pragma once
#include "GameEngine.h"

namespace Games {
    class CodeBreaker : public GameEngine {
    private:
        vector<int> secrectCode;
        vector<int> playerGuess;
        unordered_map<int, int> displayGuesses;

    protected: 
        void keypadButtonReleased(int button);
        void restartGame();
        void render();
        void generateCode(int codeLength);
        int  codeLength();

    public:
        CodeBreaker();
        void announceGame();
        void announceSameGame();
    };
}


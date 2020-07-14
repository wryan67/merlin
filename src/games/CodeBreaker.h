#pragma once
#include "GameEngine.h"

namespace Games {
    class CodeBreaker : public GameEngine {
    private:
        vector<int> secrectCode;
        vector<int> playerGuesses;
        unordered_map<int, int> displayGuesses;

    protected: 
        void keypadButtonReleased(int button);
        void restartGame();
        void render();
        void generateCode(int codeLength);
        int  codeLength();
        bool vectorContains(vector<int> haystack, int needle);

    public:
        CodeBreaker();
        void announceGame();
        void announceSameGame();
    };
}

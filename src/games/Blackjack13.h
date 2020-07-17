#pragma once
#include "GameEngine.h"

namespace Games {
    class Blackjack13 : public GameEngine {
    private:
        vector<int> playerCards;
        vector<int> dealerCards;
        vector<int> deckCards;



    protected: 
        void keypadButtonReleased(int button, long long elapsed);
        void restartGame();
        int  dealFromDeck(vector<int>& hand, bool audiable);
        void hitMe();
        void computerTurn();
        int  computeScore(vector<int>& cards);

    public:
        Blackjack13();
        void render();
    };
}


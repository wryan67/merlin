#pragma once
#include "GameEngine.h"



namespace Games {
    class TicTacToe : public GameEngine {
    protected: 
        void keypadButtonReleased(int button);
        void clearBoard();
        void restartGame();
        void computerTurn();
        void computerMovesHere(int move);
        void playBlockedSound();

        void checkGameStatus(bool& gameOver, int& winner);

        bool checkGameStatus();

    public:
        TicTacToe();
    };
}


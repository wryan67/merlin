#pragma once
#include "GameEngine.h"



namespace Games {
    class TicTacToe : public GameEngine {
    protected: 
        void keypadButtonReleased(int button);
        void clearBoard();
        void restartGame();
        void computerTurn();

        void playBlockedSound();

        bool checkGameStatus();

    public:
        TicTacToe();
    };
}


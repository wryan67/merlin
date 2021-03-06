#pragma once
#include "GameEngine.h"



namespace Games {
    class TicTacToe : public GameEngine {
        enum GamePeices {
            EMPTY, HUMAN, COMPUTER, CAT
        };

    protected: 
        void keypadButtonReleased(int button, long long elapsed);
        void clearBoard();
        void restartGame();
        void computerTurn();
        void computerMovesHere(int move);

        void checkGameStatus(bool& gameOver, int& winner);

        bool checkGameStatus();

        void render();

    public:
        TicTacToe();
    };
}


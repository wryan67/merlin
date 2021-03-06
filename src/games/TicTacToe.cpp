#include "TicTacToe.h"

namespace Games {
    TicTacToe::TicTacToe() {
        gameName = "Tic Tac Toe";
        gameWav = "tictactoe.wav";

        for (int i = 0; i < MERLIN_LIGHTS;++i) {
            keyTonesAudible[i] = false;
        }
    }

    void TicTacToe::clearBoard() {
        for (int i = 1; i <= 9; ++i) {
            pixelState[i] = EMPTY;
        }
        render();
    }

    void TicTacToe::restartGame() {
        fprintf(stderr, "%s\n", gameName);

        clearBoard();
        isActive = true;
    }

    void TicTacToe::computerTurn() {
        if (debounce(HIT_ME, 100)) return;
        bool gameOver;
        int  winner;
        vector<int> validMoves;
        if (!isActive) {
            return;
        }
        isActive = false;
        fprintf(stderr, "computer turn\n");


        for (int i = 1; i <= 9; ++i) {
            if (pixelState[i] == EMPTY) {
                validMoves.push_back(i);
            }
        }
        printf("there are %d moves left\n", validMoves.size());

// can computer win?
        for (int move : validMoves) {
            pixelState[move] = COMPUTER;
            checkGameStatus(gameOver, winner);

            if (gameOver && winner == COMPUTER) {
                computerMovesHere(move);
                return;
            }
            pixelState[move] = EMPTY;
        }

// can human win?
        for (int move : validMoves) {
            pixelState[move] = HUMAN;
            checkGameStatus(gameOver, winner);

            if (gameOver && winner == HUMAN) {
                computerMovesHere(move);
                return;
            }
            pixelState[move] = EMPTY;
        }

// is Middle open?
        for (int move : validMoves) {
            if (move == 5) {
                if (random(1, 2) == 1) {
                    computerMovesHere(move);
                    return;
                }
            }
        }


        if (validMoves.size() > 0) {
            int move = validMoves[random(0, validMoves.size()-1)];
            computerMovesHere(move);
            return;
        }

        isActive = checkGameStatus();
    }

    void TicTacToe::computerMovesHere(int move) {
        pixelState[move] = COMPUTER;
        render();
        playWav("computermove.wav", true);
        isActive = checkGameStatus();
    }

    void TicTacToe::checkGameStatus(bool& gameOver, int& winner) {
        gameOver = false;
        winner = CAT;

        for (int row = 1; !gameOver && row <= 7; row += 3) {
            if ((pixelState[row] == pixelState[row + 1]) && (pixelState[row + 1] == pixelState[row + 2])) {
                winner = pixelState[row];
                if (winner > 0) {
                    gameOver = true;
                }
            }
        }

        for (int col = 1; !gameOver && col <= 3; ++col) {
            if ((pixelState[col] == pixelState[col + 3]) && (pixelState[col + 3] == pixelState[col + 6])) {
                winner = pixelState[col];
                if (winner > 0) {
                    gameOver = true;
                }
            }
        }

        if (!gameOver && (pixelState[1] == pixelState[5]) && (pixelState[5] == pixelState[9])) {
            winner = pixelState[5];
            if (winner > 0) {
                gameOver = true;
            }
        }

        if (!gameOver && (pixelState[3] == pixelState[5]) && (pixelState[5] == pixelState[7])) {
            winner = pixelState[5];
            if (winner > 0) {
                gameOver = true;
            }
        }

        if (!gameOver) {
            bool validMovesLeft = false;
            for (int i = 1; i <= 9; ++i) {
                if (pixelState[i] == 0) {
                    validMovesLeft = true;
                    break;
                }
            }
            if (!validMovesLeft) {
                gameOver = true;
                winner = CAT;
            }
        }
    }

    bool TicTacToe::checkGameStatus() {
        bool gameOver;
        int  winner;

        checkGameStatus(gameOver, winner);

        if (gameOver) {
            if (winner == HUMAN) {
                printf("game over, human won!\n");
                playAchivement();
            }
            if (winner == COMPUTER) {
                printf("game over, computer won!\n");
                playFailed();
            }
            if (winner == CAT) {
                printf("game over, cat won!\n");
                playWav("catwon.wav", true);
            }
        }
        return !gameOver;
    }

    void TicTacToe::render() {
        for (int i = 1; i <= 9; ++i) {
            switch (pixelState[i]) {
            case EMPTY:      setPixelColor(i, -1); 
                break;
            case HUMAN:      setPixelColor(i, playerColor);
                break;
            case COMPUTER:   setPixelColor(i, computerColor);
                break;
            }
        }
        pixelColor[pixelMap[0]] = OFF;
        pixelColor[pixelMap[10]] = OFF;

        GameEngine::render();
    }

    void TicTacToe::keypadButtonReleased(int button, long long elapsed) {  // human move
        render();
        if (elapsed < 200) {
            return;
        }

        isActive = false;
        if (button<1 || button>9) {
            playWav("buzzer.wav",true);
            isActive = true;
            return;
        }

        if (pixelState[button] != EMPTY) {
            playWav("buzzer.wav", true);
            isActive = true;
            return;
        }

        pixelState[button] = HUMAN;
        render();
        keyTone(button);

        isActive = checkGameStatus();
        if (isActive) {
            isActive = false;
            usleep(600 * 1000);
            isActive = true;
            computerTurn();
        }
    }

}
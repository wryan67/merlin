#include "TicTacToe.h"
#include <thread>


namespace Games {
    TicTacToe::TicTacToe() {
        gameName = "Tic Tac Toe";
        gameWav = "tictactoe.wav";

        for (int i = 0; i < MERLIN_LIGHTS;++i) {
            keyTonesAudible[i] = false;
        }
    }

    void TicTacToe::clearBoard() {
        pixelColor[pixelMap[0]] = OFF;
        for (int i = 1; i <= 9; ++i) {
            pixelState[i] = EMPTY;
            pixelColor[pixelMap[i]] = OFF; // (pixelState[i] == 0) ? neopixel_wheel(BLUE) : neopixel_wheel(GREEN);
        }
        pixelColor[pixelMap[10]] = OFF;
        render();
    }

    void TicTacToe::restartGame() {
        fprintf(stderr, "%s\n", gameName);

        clearBoard();
        isActive = true;
    }

    void TicTacToe::computerTurn() {
        fprintf(stderr, "computer turn\n");

        vector<int> validMoves;

        for (int i = 1; i <= 9; ++i) {
            if (pixelState[i] == EMPTY) {
                validMoves.push_back(i);
            }
        }
        printf("there are %d moves left\n", validMoves.size());

        if (validMoves.size() > 0) {
            int move = validMoves[random(0, validMoves.size()-1)];
            pixelState[move] = COMPUTER;
            pixelColor[pixelMap[move]] = neopixel_wheel(BLUE);
            render();
            playWav("computermove.wav");
        }

        isActive = checkGameStatus();
    }

    void TicTacToe::playBlockedSound() {  
        playTone(globalSoundCardHandle, 123.47, 0.25, &wavHeader);
        usleep(50 * 1000);
        playTone(globalSoundCardHandle, 123.47, 0.55, &wavHeader);
    }

    bool TicTacToe::checkGameStatus() {
        bool gameOver = false;
        int  winner;


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
                playWav("catwon.wav");
            }
        }
        return !gameOver;
    }


    void TicTacToe::keypadButtonReleased(int button) {  // human move
        isActive = false;
        if (button < 1 || button>9) {
            playBlockedSound();
            pixelColor[pixelMap[button]] = 0;
            render();
            isActive = true;
            return;
        }
        if (pixelState[button] != 0) {
            playBlockedSound();
            pixelColor[pixelMap[button]] = (pixelState[button] == 1) ? neopixel_wheel(GREEN) : neopixel_wheel(BLUE);;
            render();
            isActive = true;
            return;
        }

        pixelState[button] = HUMAN;
        pixelColor[pixelMap[button]] = neopixel_wheel(GREEN);
        render();
        keyTone(button);

        isActive = checkGameStatus();
    }

}
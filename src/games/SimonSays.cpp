#include "SimonSays.h"

namespace Games {
    SimonSays::SimonSays() {
        gameName = "Simon Says";
        gameWav = "simonsays.wav";
    }

    void SimonSays::announceGame() {
        clearBoard();
        playWav(gameWav, false);
    }
    void SimonSays::announceSameGame() {
        if (isPlaying) {
            interrupt();
        }

        clearBoard();
        playWav("samegame.wav", false);
    }


    void SimonSays::restartGame() {
        printf("%s\n", gameName); fflush(stdout);

        humanSequence.clear();
        computerSequence.clear();
        GenerateNextSequence();
        isActive = true;

        isActive = true;
        isPlaying = false;
        interruptFlag = false;

        printf("%s-computer turn\n", gameName); fflush(stdout);
        clearBoard();
        computerTurn();
    }


    void SimonSays::GenerateNextSequence() {
        computerSequence.push_back(random(1, 9));
        char message[128];
        sprintf(message, "level %d", computerSequence.size());
        printf("GenerateNextSequence::%s\n", message);
        eSpeak(message);

        bool first = true;
        printf("computerSequence: ");
        for (int i : computerSequence) {
            if (!first) {
                printf(",");
            } else {
                first = false;
            }
            printf("%d", i);
        }   printf("\n");

    }


    void SimonSays::keypadButtonReleased(int button) {
        clearBoard();
        humanSequence.push_back(button);

        for (int i = 0; i < humanSequence.size(); ++i) {
            if (humanSequence[i] != computerSequence[i]) {
                playWav("buzzer.wav",true);
                humanSequence.clear();
                return;
            }
        }

        if (humanSequence.size() < computerSequence.size()) {
            return;
        }

        playWav("correct.wav", false);
        GenerateNextSequence();
        computerTurn();
    }

    void SimonSays::computerTurn() {
        if (isPlaying) {
            interruptFlag = true;
            usleep(333 * 1000);
            interruptFlag = false;
            isActive = true;
            isPlaying = false;
            clearBoard();
            return;
        }


        clearBoard();
        isActive = false;
        isPlaying = true;
        humanSequence.clear();

        for (int i : computerSequence) {
            if (interruptFlag || isActive) {
                isPlaying = false;
                isActive = true;
                return;
            }
            setPixelColor(i, keyFlashColor);
            render();
            playTone(globalSoundCardHandle, noteHz[i], .333, &wavHeader);
            usleep(1000);
            setPixelColor(i, -1);
        }

        render();
        isActive = true;
        isPlaying = false;
    }

}
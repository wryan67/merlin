#include "Game.h"
#include <alsa/asoundlib.h>
#include "tones.h"
#include <neopixel.h>
#include <unordered_map>
#include <vector>

using namespace std;

namespace Games {
    wavHeaderType wavHeader;
    int sampleRate = 48000;

    unordered_map<int, int> pixelMap;
    MCP23x17_GPIO keypadButton[MERLIN_LIGHTS];

    snd_pcm_t* keypadSoundHandle[MERLIN_LIGHTS];

    float noteHz[maxNotes] = {
      0,        // rest
      195.9977, // 1  G3
      261.6256, // 3  C4
      293.6648, // 4  D4
      329.6276, // 5  E4
      349.2282, // 6  F4
      391.9954, // 7  G4
      440.0000, // 8  A4
      493.8833, // 9  B4
      523.2511, // 10 C5
      587.3295, // 11 D5
    };

    int pixelColor[MERLIN_LIGHTS];   // color of each pixel
    int pixelState[MERLIN_LIGHTS];   // state of keypad pixel



    Game::Game() {
        initWavHeader();

        for (int i = 0; i < 11; ++i) {
            char name[32];
            sprintf(name, "KEYPAD_%d", i);
            keypadButton[i] = getEnvMCP23x17_GPIO(name);
        }

        pixelMap[0] = 3;
        pixelMap[1] = 2;
        pixelMap[2] = 4;
        pixelMap[3] = 10;
        pixelMap[4] = 1;
        pixelMap[5] = 5;
        pixelMap[6] = 9;
        pixelMap[7] = 0;
        pixelMap[8] = 6;
        pixelMap[9] = 8;
        pixelMap[10] = 7;

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            keypadSoundHandle[i] = openSoundCard(getenv("AUDIODEV"));
        }

    }



    void Game::render() {
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            neopixel_setPixel(i, pixelColor[i]);
        }
        neopixel_render();
    }

    void Game::initWavHeader() {
        strncpy(wavHeader.chunkID, "RIFF", 4);
        strncpy(wavHeader.format, "WAVE", 4);
        strncpy(wavHeader.subChunk1ID, "fmt ", 4);
        wavHeader.fileSize = 9;
        wavHeader.subChunk1Size = 16;
        wavHeader.audioFormat = 1;
        wavHeader.channels = 1;
        wavHeader.sampleRate = sampleRate;
        wavHeader.bitsPerSample = 16;
        wavHeader.blockAlign = wavHeader.bitsPerSample / 8;
        wavHeader.byteRate = wavHeader.sampleRate * wavHeader.channels * wavHeader.blockAlign;

    }


    void Game::playAchivement() {
        printf("game achievement!\n");
        char cmd[256];
        sprintf(cmd, "play %s/projects/merlin/wav/achievement-00.wav 2> /dev/null &", getenv("HOME"));
        system(cmd);
    }

    void* buttonTone(void* args) {
        int button = *((int*)args);
        snd_pcm_t* soundCardHandle = keypadSoundHandle[button];
        free(args);

        if (button < maxNotes) {
            playTone(soundCardHandle, noteHz[button], .45, &wavHeader);
        }
        pthread_exit(0);
    }

    
    void Game::restartGame() {
        randomizeBoard();
    }

    void Game::initPixels() {
        for (int c=0;c<256;++c) {
            for (int i = 0; i <= 10; ++i) {
                pixelColor[pixelMap[i]] = neopixel_wheel(c);
            }
            usleep(10 * 1000);
            render();
        }
        for (int i = 0; i <= 10; ++i) {
            pixelColor[pixelMap[i]] = 0;
        }
    }


    void Game::swapKey(int i) {
        //vector<int> todo;

        switch (i) {
        case 1:  swapState(1); swapState(2); swapState(4); swapState(5); break;
        case 3:  swapState(2); swapState(3); swapState(5); swapState(6); break;
        case 7:  swapState(7); swapState(8); swapState(4); swapState(5); break;
        case 9:  swapState(8); swapState(9); swapState(5); swapState(6); break;

        case 2:  swapState(1); swapState(2); swapState(3); break;
        case 4:  swapState(1); swapState(4); swapState(7); break;
        case 6:  swapState(3); swapState(6); swapState(9); break;
        case 8:  swapState(7); swapState(8); swapState(9); break;

        case 5:  swapState(5); swapState(2); swapState(4); swapState(6); swapState(8); break;
        default: pixelColor[pixelMap[i]] = 0;

        }
        render();
        if (pixelState[1] == 1 &&
            pixelState[2] == 1 &&
            pixelState[3] == 1 &&
            pixelState[4] == 1 &&
            pixelState[5] == 0 &&
            pixelState[6] == 1 &&
            pixelState[7] == 1 &&
            pixelState[8] == 1 &&
            pixelState[9] == 1
            ) {
            isActive = false;
            playAchivement();
        }
    }

    void Game::swapState(int i) {
        if (pixelState[i] == 1) {
            pixelState[i] = 0;
            pixelColor[pixelMap[i]] = neopixel_wheel(BLUE);
        } else {
            pixelState[i] = 1;
            pixelColor[pixelMap[i]] = neopixel_wheel(GREEN);
        }
    }

    void Game::randomizeBoard() {
        for (int i = 1; i <= 9; ++i) {
            pixelState[i] = random(0, 1);
            pixelColor[pixelMap[i]] = (pixelState[i] == 0) ? neopixel_wheel(BLUE) : neopixel_wheel(GREEN);
        }
        render();
        isActive = true;
    }

    void Game::setPixelColor(int button, int color) {
        pixelColor[pixelMap[button]] = color;
    }



    void Game::keypadButtonActivation(MCP23x17_GPIO gpio, int value) {

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            if (gpio == keypadButton[i]) {
                printf("keypad key=%d button activation port=%c pin=%d value=%d\n", i, mcp23x17_getPort(gpio) + 'A', mcp23x17_getPin(gpio), value);
                if (value == 0) {
                    if (isActive) {
                        setPixelColor(i, neopixel_wheel(RED));

                    }

                    int* button = (int*)malloc(sizeof(int));
                    *button = i;
                    threadCreate(buttonTone, "buttonTone", button);

                }
                else {
                    if (isActive) {
                        swapKey(i);
                    }
                }
            }
        }
        render();
    }
};


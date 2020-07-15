#include "GameEngine.h"
#include <thread>

using namespace std;

namespace Games {
    int      GameEngine::brightness=16;
    int      GameEngine::keyFlashColor=88;
    int      GameEngine::playerColor=248;
    int      GameEngine::computerColor=168;



    GameEngine::~GameEngine() {}
    GameEngine::GameEngine() {
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

        globalSoundCardHandle = openSoundCard(getenv("AUDIODEV"));

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            keypadSoundHandle[i] = openSoundCard(getenv("AUDIODEV"));
        }
        gameName = "undefined game name";
        gameWav  = "undefined";
    }
    void GameEngine::clearBoard() {
        for (int i = 0; i <= 10; ++i) {
            setPixelColor(i, -1);
        }
        render();
    }



    /*
    bool GameEngine::vectorContains(vector<int>& haystack, int needle) {
        return (std::find(haystack.begin(), haystack.end(), needle) != haystack.end());
    }
    */

    void GameEngine::announceGame() {
        clearBoard();
        playWav(gameWav, true);
    }

    void GameEngine::announceSameGame() {
        clearBoard();
        playWav("samegame.wav", true);
    }

    void GameEngine::render() {
        if (debug) fprintf(stderr, "render: ");

        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            if (debug) fprintf(stderr, "[%d:%d]", i, pixelColor[pixelMap[i]]);
            neopixel_setPixel(i, pixelColor[i]);
        }
        if (debug) {
            fprintf(stderr,"\n"); fflush(stderr);
        }
    
        neopixel_render();
    }

    void GameEngine::initWavHeader() {
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


    void GameEngine::playAchivement() {
        printf("game achievement!\n");
        char cmd[256];
        sprintf(cmd, "play %s/projects/merlin/wav/achievement-00.wav 2> /dev/null &", getenv("HOME"));
        system(cmd);
    }

    void GameEngine::playFailed() {
        printf("player lost!\n");
        char cmd[256];
        sprintf(cmd, "play %s/projects/merlin/wav/youlose.wav 2> /dev/null &", getenv("HOME"));
        system(cmd);
    }

    void GameEngine::setPixelColor(int button, int wheelColor) {
        if (wheelColor < 0) {
            pixelColor[pixelMap[button]] = OFF;
        } else if (wheelColor>255) {
            pixelColor[pixelMap[button]] = 0xFFFFFFFF;  // White
        } else {
            pixelColor[pixelMap[button]] = neopixel_wheel(wheelColor);
        }
    }

    int GameEngine::getPixelColor(int button) {
        return pixelColor[pixelMap[button]];
    }

    void GameEngine::playWav(const char *filename, bool background) {
        char cmd[256];
        sprintf(cmd, "play %s/projects/merlin/wav/%s 2> /dev/null %s", getenv("HOME"), filename, (background)?"&":"");
        system(cmd);
    }


    void* buttonTone(float freq, wavHeaderType *wavHeader, snd_pcm_t* soundCardHandle) {
        playTone(soundCardHandle, freq, .45, wavHeader);
    }

    void GameEngine::keyTone(int button) {
        new thread(buttonTone, noteHz[button], &wavHeader, keypadSoundHandle[button]);
    }

    void GameEngine::interrupt() {
        printf("game interrupt\n");
        interruptFlag = true;
        delay(500);
    }
    

    void GameEngine::initPixels() {
        neopixel_setBrightness(brightness);

        for (int c=0;c<256;++c) {
            for (int i = 0; i <= 10; ++i) {
                pixelColor[pixelMap[i]] = neopixel_wheel(c);
            }
            usleep(5 * 1000);
            render();
        }
        for (int i = 0; i <= 10; ++i) {
            pixelColor[pixelMap[i]] = 0;
        }
    }

    void GameEngine::eSpeak(char *message) {
        char cmd[1024];
        sprintf(cmd, "espeak --stdout %c%s%c 2>/dev/null| play -q - >/dev/null 2>&1", 39, message, 39);
        system(cmd);
    }



    void GameEngine::restartGame() {
        fprintf(stderr, "restartGame not defined: %s\n", gameName);
    }

    void GameEngine::keypadButtonReleased(int button) {
        fprintf(stderr, "keypad button released not defined: %s\n", gameName);
    }

    void GameEngine::hitMe() {
    }

    void GameEngine::computerTurn() {
    }


    void GameEngine::keypadButtonActivation(MCP23x17_GPIO gpio, int value) {
        if (debug) printf("game engine button activation isActive=%d\n",isActive);
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            if (gpio == keypadButton[i]) {
                if (debug) printf("keypad value=%d button activation port=%c pin=%d value=%d\n", i, mcp23x17_getPort(gpio) + 'A', mcp23x17_getPin(gpio), value);
                if (value == 0) {
                    if (isActive) {
                        setPixelColor(i, keyFlashColor);
                        render();
                    }

                    if (i < maxNotes && keyTonesAudible[i]) {
                        keyTone(i);
                    }
                } else {
                    if (isActive) {
                        keypadButtonReleased(i);
                    }
                }
            }
        }
    }


    bool GameEngine::vectorContains(vector<int> haystack, int needle) {
        for (int i : haystack) {
            if (i == needle) {
                return true;
            }
        }
        return false;
    }

    bool GameEngine::mapContainsValue(unordered_map<int, int> &map, int value) {

        for (pair<int, int> e : map) {
            if (e.second == value) {
                return true;
            }
        }

        return false;
    }




    void GameEngine::printVector(const char* message, vector<int>& cards) {
        bool first = true;
        printf("%s: ", message);
        for (int i : cards) {
            if (!first) {
                printf(",");
            }
            else {
                first = false;
            }
            printf("%d", i);
        }
        printf("\n");
    }


};


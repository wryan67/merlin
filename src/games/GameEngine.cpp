#include "GameEngine.h"
#include <thread>
#include <mutex>

using namespace std;

namespace Games {
    int      GameEngine::brightness=16;
    int      GameEngine::keyFlashColor=88;
    int      GameEngine::playerColor=248;
    int      GameEngine::computerColor=168;

    mutex screenLock;

    volatile long long lastActivation[MERLIN_BUTTONS];
    volatile int lastPress= -1;


    GameEngine::~GameEngine() {}
    GameEngine::GameEngine() {
        initWavFormat();

        for (int i = 0; i < 11; ++i) {
            char name[32];
            sprintf(name, "KEYPAD_%d", i);
            keypadButton[i] = getEnvMCP23x17_GPIO(name);
        }

        for (int i = 0; i < MERLIN_BUTTONS; ++i) {
            lastActivation[i] = 0;
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
        screenLock.lock();
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            neopixel_setPixel(i, pixelColor[i]);
        }
    
        neopixel_render();
        screenLock.unlock();
    }

    void GameEngine::initWavFormat() {
        wavForamt.audioFormat = 1;
        wavForamt.channels = 1;
        wavForamt.sampleRate = sampleRate;
        wavForamt.bitsPerSample = 16;
        wavForamt.blockAlign = wavForamt.bitsPerSample / 8;
        wavForamt.byteRate = wavForamt.sampleRate * wavForamt.channels * wavForamt.blockAlign;
    }


    void GameEngine::playAchivement() {
        printf("game achievement!\n");
//        char cmd[256];
//        sprintf(cmd, "play %s/projects/merlin/wav/achievement-00.wav 2> /dev/null &", getenv("HOME"));
//        system(cmd);
playWav("achievement-00.wav", true);
    }

    void GameEngine::playFailed() {
        printf("player lost!\n");
        //        char cmd[256];
        //        sprintf(cmd, "play %s/projects/merlin/wav/youlose.wav 2> /dev/null &", getenv("HOME"));
        //        system(cmd);
        playWav("youlose.wav", true);
    }

    void GameEngine::setPixelColor(int button, int wheelColor) {
        if (wheelColor < 0) {
            pixelColor[pixelMap[button]] = OFF;
        }
        else if (wheelColor > 255) {
            pixelColor[pixelMap[button]] = 0xFFFFFFFF;  // White
        }
        else {
            pixelColor[pixelMap[button]] = neopixel_wheel(wheelColor);
        }
    }

    int GameEngine::getPixelColor(int button) {
        return pixelColor[pixelMap[button]];
    }

    void GameEngine::playWav(const char* filename, bool background) {
        char* path = (char*)malloc(512 + strlen(filename));
        float volume = 1.0;
        sprintf(path, "%s/projects/merlin/wav/%s", getenv("HOME"), filename);

        //        sprintf(cmd, "play %s/projects/merlin/wav/%s 2> /dev/null %s", getenv("HOME"), filename, (background)?"&":"");
        //        system(cmd);

        if (background) {
            thread(playWavFile, path, volume).detach();
        } else {
            playWavFile(path, volume);
        }

    }


    void* buttonTone(float freq, wavFormat* wavFormat) {
        playTone(freq, .45, wavFormat);
    }

    void GameEngine::keyTone(int button) {
        thread(buttonTone, noteHz[button], &wavForamt).detach();
    }

    void GameEngine::interrupt() {
        printf("game interrupt\n");
        interruptFlag = true;
        delay(500);
    }


    void GameEngine::initPixels() {
        neopixel_setBrightness(brightness);

        for (int c = 0; c < 256; ++c) {
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

    void GameEngine::eSpeak(char* message) {
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


    bool GameEngine::debounce(int button, int ms) {
        long long currentTime = currentTimeMillis();
        if ((currentTime - lastActivation[button]) < ms) {
            return true;
        }
        lastActivation[button] = currentTime;
        return false;
    }

    void GameEngine::hitMe() {
    }

    void GameEngine::computerTurn() {
    }

    void GameEngine::keypadButtonActivation(MCP23x17_GPIO gpio, int value) {
        long long currentTime = currentTimeMillis();
        
        for (int i = 0; i < MERLIN_LIGHTS; ++i) {
            if (gpio == keypadButton[i]) {
                if (value == 0) {
                    if ((currentTime - lastActivation[i]) < 50) {
                        return;
                    }
                    lastPress = i;
                    lastActivation[i] = currentTime;
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

